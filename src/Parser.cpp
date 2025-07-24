#include "../include/Parser.hpp"

Parser::Parser(const std::vector<Token>& tkns)
    : tokens(tkns), position(0), current(this->tokens[this->position]) {}

void Parser::next_token(){
    this->position++;
    this->current = this->tokens[this->position];
}

std::optional<Token> Parser::match(TokenType type, std::optional<std::string> value = std::nullopt){
    if(this->current.type == type
        && (!value.has_value() || this->current.value == value.value())){
        Token token = this->current;
        this->next_token();
        return token;
    }
    return std::nullopt;
}

Token Parser::expect(TokenType type, const std::optional<std::string>& value = std::nullopt){
    std::optional<Token> token = this->match(type, value);
    if(!token.has_value()){
        throw std::runtime_error(
            "Expected token " + tokenTypeToString(type) + 
            " value: " + (value.has_value() ? value.value() : "") +
            " but got " + tokenTypeToString(this->current.type) + " " +
            this->current.value
        );
    }
    return token.value();
}

std::shared_ptr<ProgramStmt> Parser::parse(){
    std::vector<std::shared_ptr<Stmt>> statements;
    while(this->current.type != TokenType::END_OF_FILE){
        statements.push_back(this->parseStatement());
    }
    return std::make_shared<ProgramStmt>(statements);
}

std::shared_ptr<Stmt> Parser::parseStatement(){
    if(this->match(TokenType::VAR).has_value()) return this->parseVarDeclStmt();
    if(this->match(TokenType::PRINT).has_value()) return this->parsePrintStmt();
    if(this->match(TokenType::IF).has_value()) return this->parseIfStmt();
    if(this->match(TokenType::FOR).has_value()) return this->parseForStmt();
    if(this->match(TokenType::RETURN).has_value()) return this->parseReturnStmt();
    if(this->match(TokenType::FUN).has_value()) return this->parseFunctionDeclStmt();
    if(this->current.type == TokenType::IDENTIFIER) return this->parseIdentifierStmt();

    throw std::runtime_error("Unexpected token: " + tokenTypeToString(this->current.type));
}

std::shared_ptr<VarDeclStmt> Parser::parseVarDeclStmt(){
    std::string name = this->expect(TokenType::IDENTIFIER).value;
    this->expect(TokenType::ASSIGN, "=");
    std::shared_ptr<Stmt> value = this->parseExpression();
    this->expect(TokenType::SEMICOLON, ";");
    return std::make_shared<VarDeclStmt>(name, value);
}


std::shared_ptr<PrintStmt> Parser::parsePrintStmt(){
    this->expect(TokenType::LPAREN);
    std::shared_ptr<Stmt> expr = this->parseExpression();
    this->expect(TokenType::RPAREN);
    this->expect(TokenType::SEMICOLON);
    return std::make_shared<PrintStmt>(expr);
}


std::shared_ptr<IfStmt> Parser::parseIfStmt(){
    std::shared_ptr<Stmt> condition = this->parseExpressionUntil(TokenType::LBRACE);
    std::vector<std::shared_ptr<Stmt>> thenBlock = this->parseBlock();
    std::vector<std::shared_ptr<ElifStmt>> elifs;
    std::vector<std::shared_ptr<Stmt>> elseBlock;

    while(this->match(TokenType::ELIF)){
        std::shared_ptr<Stmt> elifCondition = this->parseExpressionUntil(TokenType::LBRACE);
        std::vector<std::shared_ptr<Stmt>> elifBody = this->parseBlock();
        elifs.push_back(std::make_shared<ElifStmt>(elifCondition, elifBody));
    }

    if(this->match(TokenType::ELSE)){
        elseBlock = this->parseBlock();
    }

    return std::make_shared<IfStmt>(condition, thenBlock, elifs, elseBlock);
}


std::shared_ptr<ForStmt> Parser::parseForStmt(){
    std::string iterator = this->expect(TokenType::IDENTIFIER).value;
    this->expect(TokenType::ASSIGN, "=");
    std::shared_ptr<Stmt> start = this->parseExpression();
    this->expect(TokenType::RANGE, "..");
    std::shared_ptr<Stmt> end = this->parseExpressionUntil(TokenType::LBRACE);
    std::vector<std::shared_ptr<Stmt>> body = this->parseBlock();

    int step = 1;
    if (auto startLit = std::dynamic_pointer_cast<LiteralStmt>(start)) {
        if (auto endLit = std::dynamic_pointer_cast<LiteralStmt>(end)) {
            int startVal = std::stoi(startLit->value);
            int endVal = std::stoi(endLit->value);
            if (startVal > endVal) step = -1;
        }
    }
    std::shared_ptr<Stmt> stepStmt = std::make_shared<LiteralStmt>(std::to_string(step));
    return std::make_shared<ForStmt>(iterator, start, end, stepStmt, body);    
}


std::shared_ptr<ReturnStmt> Parser::parseReturnStmt(){
    // return; <-- tanpa nilai
    if (this->current.type == TokenType::SEMICOLON) {
        this->expect(TokenType::SEMICOLON);
        return std::make_shared<ReturnStmt>(nullptr);
    }

    // return <expression>;
    std::shared_ptr<Stmt> value = this->parseExpression(); // misal return 1 + 2;
    this->expect(TokenType::SEMICOLON);
    return std::make_shared<ReturnStmt>(value);
}


std::shared_ptr<FunctionDeclStmt> Parser::parseFunctionDeclStmt(){
    std::string name = this->expect(TokenType::IDENTIFIER).value;
    this->expect(TokenType::LPAREN);
    std::vector<std::string> params;
    if(this->current.type == TokenType::IDENTIFIER){
        params.push_back(this->expect(TokenType::IDENTIFIER).value);
        while(this->match(TokenType::COMMA)){
            params.push_back(this->expect(TokenType::IDENTIFIER).value);
        }
    }
    
    this->expect(TokenType::RPAREN);
    std::vector<std::shared_ptr<Stmt>> body = this->parseBlock();
    // console.log("body: ", body);
    
    return std::make_shared<FunctionDeclStmt>(name, params, body);
}


std::shared_ptr<FunctionCallStmt> Parser::parseFunctionCallStmt(std::shared_ptr<Stmt> callee) {
    auto ident = std::dynamic_pointer_cast<IdentifierStmt>(callee);
    if (!ident) {
        throw std::runtime_error("Function call must begin with an identifier");
    }

    std::vector<std::shared_ptr<Stmt>> args;

    // Cek jika token saat ini bukan RPAREN, berarti ada argumen
    if (this->current.type != TokenType::RPAREN) {
        args.push_back(this->parseExpression());
        while (this->match(TokenType::COMMA)) {
            args.push_back(this->parseExpression());
        }
    }

    // Harus expect RPAREN di akhir
    this->expect(TokenType::RPAREN, ")");

    return std::make_shared<FunctionCallStmt>(ident->name, args);
}



std::shared_ptr<Stmt> Parser::parseIdentifierStmt(){
    std::shared_ptr<Stmt> id = std::make_shared<IdentifierStmt>(this->current.value);
    this->next_token();
    
    if(this->match(TokenType::LPAREN)) {
        std::shared_ptr<Stmt> call = this->parseFunctionCallStmt(id);
        this->expect(TokenType::SEMICOLON);
        return call;
    }
    
    throw std::runtime_error("Unexpected identifier usage: " + this->current.value);
}


std::vector<std::shared_ptr<Stmt>> Parser::parseBlock(){
    this->expect(TokenType::LBRACE);
    std::vector<std::shared_ptr<Stmt>> statements;
    while(this->current.type != TokenType::RBRACE){
        statements.push_back(this->parseStatement());
    }
    this->expect(TokenType::RBRACE);
    return statements;
}


std::shared_ptr<Stmt> Parser::parseExpressionUntil(TokenType endType){
    std::vector<Token> exprTokens;
    while(this->current.type != TokenType::END_OF_FILE &&
          this->current.type != endType){
        exprTokens.push_back(this->current);
        this->next_token();
    }
 
    // Tambahkan token EOF sebagai penanda akhir input untuk sub-parser
    exprTokens.push_back(Token{TokenType::END_OF_FILE, ""});

    // Buat sub-parser dan parse ekspresinya
    Parser subParser(exprTokens); // pastikan kamu punya constructor Parser(vector<Token>)
    return subParser.parseExpression(); // ini harus mengembalikan shared_ptr<Stmt>
}

std::shared_ptr<Stmt> Parser::parseExpression(){
    return this->parseEquality();
}


std::shared_ptr<Stmt> Parser::parseEquality(){
    std::shared_ptr<Stmt> left = this->parseComparison();
    while(this->match(TokenType::COMPARISON, "==") || this->match(TokenType::COMPARISON, "!=")){
        std::string op = this->tokens[this->position - 1].value;
        std::shared_ptr<Stmt> right = this->parseComparison();
        left = std::make_shared<BinaryOpStmt>(left, op, right);
    }
    return left;
}


std::shared_ptr<Stmt> Parser::parseComparison(){
    std::shared_ptr<Stmt> left = this->parseTerm();
    while(this->match(TokenType::COMPARISON)){
        std::string op = this->tokens[this->position - 1].value;;
        std::shared_ptr<Stmt> right = this->parseTerm();
        left = std::make_shared<BinaryOpStmt>(left, op, right);
    }
    return left;
}


std::shared_ptr<Stmt> Parser::parseTerm(){
    std::shared_ptr<Stmt> left = this->parseFactor();
    while(this->match(TokenType::OPERATOR, "+") || this->match(TokenType::OPERATOR, "-")){
        std::string op = this->tokens[this->position - 1].value;
        std::shared_ptr<Stmt> right = this->parseFactor();
        left = std::make_shared<BinaryOpStmt>(left, op, right);
    }
    return left;
}


std::shared_ptr<Stmt> Parser::parseFactor(){
    std::shared_ptr<Stmt> left = this->parsePrimary();
    while(this->match(TokenType::OPERATOR, "*") || this->match(TokenType::OPERATOR, "/")){
        std::string op = this->tokens[this->position - 1].value;
        std::shared_ptr<Stmt> right = this->parsePrimary();
        left = std::make_shared<BinaryOpStmt>(left, op, right);
    }
    return left;
}


// std::shared_ptr<Parser> Parser::parseUnary(){}


std::shared_ptr<Stmt> Parser::parsePrimary(){
    if(this->current.type == TokenType::INT ||
        this->current.type == TokenType::FLOAT ||
        this->current.type == TokenType::STRING ||
        this->current.type == TokenType::BOOLEAN)
    {
        std::string value = this->current.value;
        this->next_token();
        return std::make_shared<LiteralStmt>(value);
    }

    // cek input
    if(this->current.type == TokenType::INPUT){
        this->next_token();
        this->expect(TokenType::LPAREN);
        this->expect(TokenType::RPAREN);
        return std::make_shared<InputStmt>();
    }

    // cek typeof
    if(this->current.type == TokenType::TYPEOF){
        // const value = this->current.value;
        this->next_token();
        std::shared_ptr<Stmt> expr = this->parsePrimary(); // cek identifier atau literal
        return std::make_shared<TypeofStmt>(expr);
    }

    // cek variabel
    if(this->current.type == TokenType::IDENTIFIER){
        std::shared_ptr<Stmt> ID = std::make_shared<IdentifierStmt>(this->current.value);
        this->next_token();

        // cek kalau pemanggilan fungsi
        if(this->match(TokenType::LPAREN)){
            return this->parseFunctionCallStmt(ID);
        }

        // cek array
        if(this->match(TokenType::LBRACKET)){
            std::shared_ptr<Stmt> index = this->parseExpression();
            this->expect(TokenType::RBRACKET);
            auto idStmt = std::dynamic_pointer_cast<IdentifierStmt>(ID);
            if (!idStmt) {
                throw std::runtime_error("Expected identifier in array access");
            }
            return std::make_shared<ArrayAccessStmt>(idStmt->name, index);
        }
        return ID;
    }

    // cek array
    if(this->match(TokenType::LBRACKET)){
        std::vector<std::shared_ptr<Stmt>> elements;

        if(this->current.type != TokenType::RBRACKET){
            elements.push_back(this->parseExpression()); // ini nanti sampai ke parsePrimary
            while(this->match(TokenType::COMMA)){
                elements.push_back(this->parseExpression());
            }
        }

        this->expect(TokenType::RBRACKET);
        return std::make_shared<ArrayLiteralStmt>(elements);
    }

    // cek (expr)
    if(this->match(TokenType::LPAREN)){
        std::shared_ptr<Stmt> expr = this->parseExpression();
        this->expect(TokenType::RPAREN);
        return expr;
    }
    throw std::runtime_error("Unexpected token in expression: " + tokenTypeToString(this->current.type));
}

