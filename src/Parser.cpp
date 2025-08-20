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
    if(this->match(TokenType::BREAK).has_value()) {
        this->expect(TokenType::SEMICOLON);
        return std::make_shared<BreakStmt>();
    }
    if(this->match(TokenType::CONTINUE).has_value()) {
        this->expect(TokenType::SEMICOLON);
        return std::make_shared<ContinueStmt>();
    }
    if(this->match(TokenType::STRUCT).has_value()) return this->parseStructDecl();

    throw std::runtime_error("Unexpected token: " + tokenTypeToString(this->current.type));
}


LiteralType mapStringToLiteralType(const std::string& s){
    if(s == "i32") return LiteralType::INT_32;
    if(s == "i64") return LiteralType::INT_64; // nanti diperluas
    if(s == "f32") return LiteralType::FLOAT;
    if(s == "f64") return LiteralType::DOUBLE;
    if(s == "bool") return LiteralType::BOOL;
    if(s == "str") return LiteralType::STRING;
    return LiteralType::UNKNOWN;
}

LiteralType inferTypeFromExpr(std::shared_ptr<Stmt> expr) {
    switch(expr->type) {
        case StmtType::Literal: {
            auto lit = std::static_pointer_cast<LiteralStmt>(expr);
            return lit->dataType;
        }
        case StmtType::BinaryOp: {
            auto binOp = std::static_pointer_cast<BinaryOpStmt>(expr);
            LiteralType leftType = inferTypeFromExpr(binOp->left);
            LiteralType rightType = inferTypeFromExpr(binOp->right);
            
            // floating point dulu, prioritas tinggi
            if(leftType == LiteralType::DOUBLE || rightType == LiteralType::DOUBLE)
                return LiteralType::DOUBLE;
            if(leftType == LiteralType::FLOAT || rightType == LiteralType::FLOAT)
                return LiteralType::FLOAT;

            // integer: pilih yang lebih besar
            if((leftType == LiteralType::INT_64 || rightType == LiteralType::INT_64))
                return LiteralType::INT_64;
            if(leftType == LiteralType::INT_32 && rightType == LiteralType::INT_32)
                return LiteralType::INT_32;


            return LiteralType::UNKNOWN;
        }
        // case StmtType::Identifier:
        //     auto id = std::static_pointer_cast<IdentifierStmt>(expr);
        //     return id->; // misal variable sudah punya tipe
        default:
            return LiteralType::UNKNOWN;
    }
}




std::shared_ptr<VarDeclStmt> Parser::parseVarDeclStmt(){
    std::string name = this->expect(TokenType::IDENTIFIER).value;

    // opsional pengecekan untuk tipe eksplisit
    LiteralType explicitType = LiteralType::UNKNOWN;
    bool hasExplicit = false;
    if(this->match(TokenType::COLON)){
        Token typeToken = this->expect(TokenType::TYPE); // TYPE = i32, f64, bool, string, dll
        explicitType = mapStringToLiteralType(typeToken.value);
        hasExplicit = true;
    }


    std::shared_ptr<Stmt> value = nullptr;
    if (this->match(TokenType::ASSIGN)) { // kalau ada '='
        value = this->parseExpression();
    }

    this->expect(TokenType::SEMICOLON, ";");
    return std::make_shared<VarDeclStmt>(name, value, explicitType, hasExplicit);
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

    int32_t step = 1;

    auto startLit = std::dynamic_pointer_cast<LiteralStmt>(start);
    auto endLit = std::dynamic_pointer_cast<LiteralStmt>(end);

    if (!startLit || !endLit)
        throw std::runtime_error("For loop start and end must be integer literals");

    // cek jika value adalah int32_t
    if (!std::holds_alternative<int32_t>(startLit->value) || 
        !std::holds_alternative<int32_t>(endLit->value))
    {
        throw std::runtime_error("For loop only supports integer literals");
    }

    int32_t startVal = std::get<int32_t>(startLit->value);
    int32_t endVal = std::get<int32_t>(endLit->value);

    if (startVal > endVal) step = -1;

    std::shared_ptr<Stmt> stepStmt = std::make_shared<LiteralStmt>(int32_t(step));

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
    
    std::vector<std::shared_ptr<Stmt>> params;
    while(this->current.type == TokenType::IDENTIFIER) {
        std::string paramName = this->expect(TokenType::IDENTIFIER).value;

        // cek tipe eksplisit setelah ':'
        LiteralType paramType = LiteralType::UNKNOWN;
        bool hasExplicit = false;
        if(this->match(TokenType::COLON)) {
            Token typeToken = this->expect(TokenType::TYPE); // misal "i32", "f64"
            paramType = mapStringToLiteralType(typeToken.value);
            hasExplicit = true;
        }

        // buat VarDeclStmt untuk parameter
        params.push_back(std::make_shared<VarDeclStmt>(paramName, nullptr, paramType, hasExplicit));

        if(!this->match(TokenType::COMMA))
            break; // tidak ada koma lagi
    }

    
    this->expect(TokenType::RPAREN);
    std::vector<std::shared_ptr<Stmt>> body = this->parseBlock();
    
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
    std::shared_ptr<Stmt> idStmt = std::make_shared<IdentifierStmt>(this->current.value);
    this->next_token();
    
    // std::cout << tokenTypeToString(this->current.type) << "\n";

    // array access
    std::shared_ptr<Stmt> index = nullptr;
    if(this->match(TokenType::LBRACKET)){
        index = this->parseExpression();
        this->expect(TokenType::RBRACKET);
    }

    // assign
    if(this->match(TokenType::ASSIGN)){
        std::shared_ptr<Stmt> expr = this->parseExpression();
        this->expect(TokenType::SEMICOLON);

        auto id = std::dynamic_pointer_cast<IdentifierStmt>(idStmt);
        return std::make_shared<AssignmentStmt>(id.get()->name, expr, index);
    }

    if(this->current.type == TokenType::OPERATOR) {
        std::string op = this->current.value; // ambil operator
        this->next_token(); // konsumsi operator
    
        if(op == "+=" || op == "-=" || op == "*=" || op == "/=" || op == "%=") {
            std::shared_ptr<Stmt> expr = this->parseExpression();
            this->expect(TokenType::SEMICOLON);
    
            auto id = std::dynamic_pointer_cast<IdentifierStmt>(idStmt);
    
            // Ubah jadi Assignment( id = BinaryOp(id, op, expr) )
            auto binary = std::make_shared<BinaryOpStmt>(
                idStmt,                   // lhs
                op.substr(0, 1),          // ambil operator dasar: "+", "-", "*", "/", "%"
                expr                      // rhs
            );
    
            return std::make_shared<AssignmentStmt>(id->name, binary, index);
        }
    }
    
    

    // function call langsung, misal untuk prosedur
    if(this->match(TokenType::LPAREN)) {
        std::shared_ptr<Stmt> call = this->parseFunctionCallStmt(idStmt);
        this->expect(TokenType::SEMICOLON);
        return call;
    }
    
    throw std::runtime_error("Unexpected identifier usage: " + this->current.value);
}

std::shared_ptr<Stmt> Parser::parseTypeof(){
    std::shared_ptr<Stmt> expr = this->parsePrimary(); // cek identifier atau literal
    return std::make_shared<TypeofStmt>(expr);
}


std::shared_ptr<Stmt> Parser::parseInput(){
    this->expect(TokenType::LPAREN);

    std::shared_ptr<Stmt> expr = parseExpression();

    this->expect(TokenType::COMMA);

    Token typeTok;
    if (this->current.type == TokenType::IDENTIFIER || this->current.type == TokenType::STRING_LITERAL) {
        typeTok = this->current;
        this->next_token();
    } else {
        throw std::runtime_error("Expected type identifier or string, got: " + tokenTypeToString(this->current.type));
    }
    std::string dataType = typeTok.value;


    this->expect(TokenType::RPAREN);
    return std::make_shared<InputStmt>(expr, dataType);
}



std::shared_ptr<Stmt> Parser::parseStructDecl(){
    std::string name = this->expect(TokenType::IDENTIFIER).value;
    this->expect(TokenType::LBRACE);
    std::vector<std::shared_ptr<StructField>> fields;

    while(this->current.type == TokenType::IDENTIFIER){
        std::string fieldName = this->expect(TokenType::IDENTIFIER).value;

        // cek tipe eksplisit setelah ':'
        LiteralType fieldType = LiteralType::UNKNOWN;
        if(!this->match(TokenType::COLON)){
            throw std::runtime_error(fieldName + " doesn't have a tipe");
        }
    
        Token typeToken = this->expect(TokenType::TYPE); // misal "i32", "f64"
        fieldType = mapStringToLiteralType(typeToken.value);

        if(fieldType == LiteralType::UNKNOWN)
            throw std::runtime_error("Unknown type for field " + fieldName);

        // buat StructField untuk parameter
        fields.push_back(std::make_shared<StructField>(fieldName, fieldType));

        if(!this->match(TokenType::COMMA))
            break; // tidak ada koma lagi
    }
    this->expect(TokenType::RBRACE);
    this->expect(TokenType::SEMICOLON, ";");
    return std::make_shared<StructStmt>(name, fields);
}





std::shared_ptr<Stmt> Parser::parseStructInit(){
    std::string name = this->expect(TokenType::IDENTIFIER).value;
    std::cout << "name: " << name << '\n';
    this->expect(TokenType::LBRACE);
    
    std::vector<std::pair<std::string, std::shared_ptr<Stmt>>> fieldsValue;

    // Loop sampai '}'
    while (this->current.type != TokenType::RBRACE) {
        std::string fieldName = this->expect(TokenType::IDENTIFIER).value; // nama field
        this->expect(TokenType::COLON);  // simbol ':'

        // Parsing ekspresi yang akan di-assign ke field
        std::shared_ptr<Stmt> value = parseExpression();

        fieldsValue.push_back({fieldName, value});

        // Optional comma ',' setelah field
        if (this->current.type != TokenType::RBRACE) {
            this->expect(TokenType::COMMA);
        }
    }

    this->expect(TokenType::RBRACE);

    return std::make_shared<StructExpr>(name, fieldsValue);
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
    std::shared_ptr<Stmt> left = this->parseUnary();
    while(this->match(TokenType::OPERATOR, "*") || this->match(TokenType::OPERATOR, "/") || this->match(TokenType::OPERATOR, "%")){
        std::string op = this->tokens[this->position - 1].value;
        std::shared_ptr<Stmt> right = this->parseUnary();
        left = std::make_shared<BinaryOpStmt>(left, op, right);
    }
    return left;
}


std::shared_ptr<Stmt> Parser::parseUnary(){
    while(this->match(TokenType::OPERATOR, "-") || this->match(TokenType::OPERATOR, "!")){
        std::string op = this->tokens[this->position - 1].value;
        std::shared_ptr<Stmt> right = this->parseUnary();
        return std::make_shared<UnaryOpStmt>(op, right);
    }
    return parsePrimary();
}


std::shared_ptr<Stmt> Parser::parsePrimary(){
    if(this->current.type == TokenType::INT_LITERAL ||
        this->current.type == TokenType::DOUBLE_LITERAL ||
        this->current.type == TokenType::STRING_LITERAL ||
        this->current.type == TokenType::BOOLEAN_LITERAL)
    {
        std::string raw = this->current.value;
        if(this->current.type == TokenType::INT_LITERAL){
            this->next_token();
            int64_t v = 0;
            auto res = std::from_chars(raw.data(), raw.data() + raw.size(), v);
            
            if(res.ec == std::errc::invalid_argument){
                throw std::runtime_error("Invalid integer literal: " + raw);
            } else if(res.ec == std::errc::result_out_of_range){
                throw std::runtime_error("Integer literal out of range: " + raw);
            }
        
            // pilih constructor LiteralStmt yang sesuai
            if(v <= INT32_MAX && v >= INT32_MIN){
                return std::make_shared<LiteralStmt>(static_cast<int32_t>(v));
            }
            else {
                return std::make_shared<LiteralStmt>(v); // INT64
            }
        }
        
        
        if(this->current.type == TokenType::DOUBLE_LITERAL) {
            this->next_token();
            double d = std::strtod(raw.c_str(), nullptr);
            return std::make_shared<LiteralStmt>(d);
        }

        if(this->current.type == TokenType::BOOLEAN_LITERAL){
            std::string raw = this->current.value; // "true" atau "false"
            this->next_token();
            bool b = (raw == "true" || raw == "1");
            return std::make_shared<LiteralStmt>(b);
        }
        
        if(this->current.type == TokenType::STRING_LITERAL){
            this->next_token();
            return std::make_shared<LiteralStmt>(raw);
        }    
    }

    // cek input
    if(this->match(TokenType::INPUT).has_value()){
        return this->parseInput();
    }

    // cek typeof
    if(this->match(TokenType::TYPEOF).has_value()){
        return parseTypeof();
    }

    
    // cek variabel
    if(this->current.type == TokenType::IDENTIFIER){
        if( this->position + 1 < tokens.size() && // untuk cek struct init
            this->tokens[this->position + 1].type == TokenType::LBRACE){
            return this->parseStructInit();            
        }
        // variabel biasa
        std::shared_ptr<Stmt> ID = std::make_shared<IdentifierStmt>(this->current.value);
        this->next_token();

        // cek dot operator (akses struct field)
        while (this->match(TokenType::DOT)) {
            if (this->current.type != TokenType::IDENTIFIER) {
                throw std::runtime_error("Expected identifier after '.'");
            }

            std::string fieldName = this->current.value;
            this->next_token();

            ID = std::make_shared<MemberAccessExpr>(ID, fieldName);
        }


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