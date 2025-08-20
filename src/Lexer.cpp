#include <iostream>
#include "../include/Lexer.hpp"

Lexer::Lexer(const std::string& input)
    : input(input), position(0), currentChar(this->input[this->position]) {}


// lanjut ke karakter selanjutnya lalu simpan ke current char
void Lexer::next_char(){
    this->position++;
    this->currentChar = (this->position < this->input.size()) ?
        this->input[this->position] : '\0';
}    


// lihat karakter
char Lexer::peek() { return this->currentChar; }


void Lexer::skip_whitespace(){
    while(this->currentChar != '\0' && std::isspace(this->currentChar)){
        this->next_char();
    }
}


void Lexer::skip_comment(){
    while(this->currentChar != '\0' && this->currentChar != '\n'){
        this->next_char();
    }
}


bool Lexer::isAlphaNumeric(char ch){ return (std::isalpha(ch) || std::isdigit(ch)); }


Token Lexer::readNumber(){
    std::string result = "";
    // selama ada karakter angka
    while(this->currentChar != '\0' && std::isdigit(this->currentChar)){
        result += this->currentChar;
        this->next_char();
    }
    // untuk mendeteksi floating point
    if(this->currentChar == '.'){
        result += this->currentChar;
        this->next_char();
        while(this->currentChar != '\0' && std::isdigit(this->currentChar)){
            result += this->currentChar;
            this->next_char();
        }
        return Token(TokenType::DOUBLE_LITERAL, result);
    }
    return Token(TokenType::INT_LITERAL, result);
}



Token Lexer::readString(){
    char quote = this->currentChar;
    this->next_char();

    std::string result = "";

    // alfabet
    while(this->currentChar != '\0' && this->currentChar != quote){
        result += this->currentChar;
        this->next_char();
        // belum ada escape sequence
    }

    if(this->currentChar == quote){
        this->next_char();
        return Token(TokenType::STRING_LITERAL, result);
    }
    else {
        throw std::runtime_error("Unterminated String Literal");
    }
}



Token Lexer::readBoolean() {
    if (this->currentChar == 't') {
        if (this->input.substr(this->position, 4) == "true") {
            for (int i = 0; i < 4; i++) this->next_char();
            return Token(TokenType::BOOLEAN_LITERAL, "true");
        }
    }

    if (this->currentChar == 'f') {
        if (this->input.substr(this->position, 5) == "false") {
            for (int i = 0; i < 5; i++) this->next_char();
            return Token(TokenType::BOOLEAN_LITERAL, "false");
        }
    }

    return Token(TokenType::INVALID, "");
}



Token Lexer::readIdentifier(){
    std::string result = "";
    while(this->currentChar && this->isAlphaNumeric(this->currentChar)){
        result += this->currentChar;
        this->next_char();
    }

    std::unordered_map<std::string, TokenType> keywords = {
        {"var", TokenType::VAR},
        {"print", TokenType::PRINT},
        {"if", TokenType::IF},
        {"elif", TokenType::ELIF},
        {"else", TokenType::ELSE},
        {"for", TokenType::FOR},
        {"fun", TokenType::FUN},
        {"return", TokenType::RETURN},
        {"..", TokenType::RANGE},
        {"typeof", TokenType::TYPEOF},
        {"input", TokenType::INPUT},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"i32", TokenType::TYPE},
        {"i64", TokenType::TYPE},
        {"f32", TokenType::TYPE},
        {"f64", TokenType::TYPE},
        {"bool", TokenType::TYPE},
        {"str", TokenType::TYPE},
        {"struct", TokenType::STRUCT},
        // {"while", TokenType::WHILE},
    };

    auto keyword = keywords.find(result);
    if (keyword != keywords.end()) {
        return Token{keyword->second, result}; // Keyword ditemukan
    }

    return Token{TokenType::IDENTIFIER, result}; // Bukan keyword
}

Token Lexer::get_next_token(){
    while(this->peek()){
        // skip whitespace
        if(std::isspace(this->currentChar)){
            this->skip_whitespace();
            continue;;
        }

        // skip comment
        if(this->currentChar == '#'){
            this->skip_comment();
            continue;
        }

        if(this->currentChar == '.' && this->position + 1 < this->input.size() && this->input[this->position + 1] == '.'){
            this->next_char();
            this->next_char();
            return Token(TokenType::RANGE, "..");
        }

        // cek boolean
        if(this->currentChar == 't' || this->currentChar == 'f'){
            Token booleanToken = this->readBoolean();
            if(booleanToken.type != TokenType::INVALID) return booleanToken;
        }

        // cek negatif
        if(this->currentChar == '-' && std::isdigit(this->input[this->position + 1])){
            this->next_char();
            Token numberToken = this->readNumber();
            numberToken.value = std::to_string(std::atoi(numberToken.value.c_str()) * -1);
            return numberToken;
        }

        // handle string
        if(this->currentChar == '"' || this->currentChar == '\''){
            return readString();
        }

        if(std::isdigit(this->currentChar)){
            return this->readNumber();
        }

        // handle identifier
        if(std::isalpha(this->currentChar)){
            return this->readIdentifier();
        }

        // handle operators
        if(this->currentChar == '+' || this->currentChar == '-' ||
            this->currentChar == '*' || this->currentChar == '/' ||
            this->currentChar == '%') {

            char first = this->currentChar;
            this->next_char();
        
            // cek apakah operator shorthand (+=, -=, *=, /=, %=)
            if(this->currentChar == '=') {
                char second = this->currentChar;
                this->next_char();
                return Token(TokenType::OPERATOR, std::string() + first + second); // "+=", dll
            }
        
            return Token(TokenType::OPERATOR, std::string(1, first));
        }

        // Comparison dan assignment
        if (this->currentChar == '>' || this->currentChar == '<' ||
            this->currentChar == '=' || this->currentChar == '!') {

            char op = this->currentChar;
            this->next_char();

            if (this->currentChar == '=') {
                std::string combined = std::string(1, op) + "=";
                this->next_char();
                return Token(TokenType::COMPARISON, combined); // ==, >=, <=, !=
            }

            if (op == '=') {
                return Token(TokenType::ASSIGN, "="); // hanya =
            }

            if (op == '>' || op == '<') {
                return Token(TokenType::COMPARISON, std::string(1, op)); // > atau <
            }
        
            if (op == '!') {
                return Token(TokenType::OPERATOR, "!"); // unary not (!)
            }

            // >, <, !
            return Token(TokenType::COMPARISON, std::string(1, op));
        }

        std::unordered_map<char, TokenType> singleCharTokens = {
            {';', TokenType::SEMICOLON},
            {'(', TokenType::LPAREN},
            {')', TokenType::RPAREN},
            {'{', TokenType::LBRACE},
            {'}', TokenType::RBRACE},
            {',', TokenType::COMMA},
            {'.', TokenType::DOT},
            {':', TokenType::COLON},
            {'[', TokenType::LBRACKET},
            {']', TokenType::RBRACKET},
        };

        auto it = singleCharTokens.find(this->currentChar);
        if (it != singleCharTokens.end()) {
            char value = this->currentChar;
            this->next_char();
            return Token(it->second, std::string(1, value));
        }

        return Token(TokenType::INVALID, ""); // Token default jika bukan karakter tunggal
    }

    return Token(TokenType::END_OF_FILE, "\0");
}

std::vector<Token> Lexer::tokenize(){
    std::vector<Token> tokens;
    Token token = this->get_next_token();
    while(token.type != TokenType::END_OF_FILE){
        tokens.push_back(token);
        token = this->get_next_token();
    }
    tokens.push_back(token);
    return tokens;
}