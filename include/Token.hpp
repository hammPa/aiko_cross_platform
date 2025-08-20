#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

enum class TokenType {
    DOUBLE_LITERAL, INT_LITERAL, STRING_LITERAL, BOOLEAN_LITERAL,
    TYPE,
    VAR,
    PRINT,
    IF,
    ELIF,
    ELSE,
    FOR,
    FUN,
    RETURN,
    RANGE,
    TYPEOF,
    INPUT,
    BREAK,
    CONTINUE,
    IDENTIFIER,
    
    OPERATOR,
    ASSIGN,
    COMPARISON,

    SEMICOLON,
    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COMMA,
    COLON,
    LBRACKET,
    RBRACKET,

    STRUCT,
    DOT,

    INVALID,
    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string value;

    Token(){}
    Token(TokenType t, const std::string& v) : type(t), value(v) {}
};

inline std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::DOUBLE_LITERAL: return "DOUBLE_LITERAL";
        case TokenType::INT_LITERAL: return "INT_LITERAL";
        case TokenType::STRING_LITERAL: return "STRING_LITERAL";
        case TokenType::BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";

        case TokenType::TYPE: return "TYPE";
        
        case TokenType::VAR: return "VAR";
        case TokenType::PRINT: return "PRINT";
        case TokenType::IF: return "IF";
        case TokenType::ELIF: return "ELIF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FOR: return "FOR";
        case TokenType::FUN: return "FUN";
        case TokenType::RETURN: return "RETURN";
        case TokenType::RANGE: return "RANGE";
        case TokenType::TYPEOF: return "TYPEOF";
        case TokenType::INPUT: return "INPUT";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        
        case TokenType::OPERATOR: return "OPERATOR";
        case TokenType::ASSIGN: return "ASSIGN";
        case TokenType::COMPARISON: return "COMPARISON";

        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::LPAREN: return "LPAREN";
        case TokenType::RPAREN: return "RPAREN";
        case TokenType::LBRACE: return "LBRACE";
        case TokenType::RBRACE: return "RBRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::COLON: return "COLON";
        case TokenType::LBRACKET: return "LBRACKET";
        case TokenType::RBRACKET: return "RBRACKET";

        case TokenType::STRUCT: return "STRUCT";
        case TokenType::DOT: return "DOT";

        case TokenType::INVALID: return "INVALID";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        
        default: return "UNKNOWN";
    }
}


#endif