#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "Token.hpp"

class Lexer {
private:
    std::string input;
    int position;
    char currentChar;
public:
    Lexer(const std::string& input);
    void next_char();
    char peek();
    void skip_whitespace();
    void skip_comment();
    bool isAlphaNumeric(char ch);
    Token readNumber();
    Token readString();
    Token readBoolean();
    Token readIdentifier();
    Token get_next_token();
    std::vector<Token> tokenize();
};

#endif