#ifndef PARSER_HPP
#define PARSER_HPP

#include "./AstTree.hpp"
#include "./Token.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <optional>
#include <cctype>

class Parser {
private:
    std::vector<Token> tokens;
    int position;
    Token current;
public:
    Parser(const std::vector<Token>&);
    void next_token();
    std::optional<Token> match(TokenType, std::optional<std::string>);
    Token expect(TokenType, const std::optional<std::string>&);
    std::shared_ptr<ProgramStmt> parse();


    std::shared_ptr<Stmt> parseStatement();
    std::shared_ptr<VarDeclStmt> parseVarDeclStmt();
    std::shared_ptr<PrintStmt> parsePrintStmt();
    std::shared_ptr<IfStmt> parseIfStmt();
    std::shared_ptr<ForStmt> parseForStmt();
    std::shared_ptr<ReturnStmt> parseReturnStmt();
    std::shared_ptr<FunctionDeclStmt> parseFunctionDeclStmt();
    std::shared_ptr<FunctionCallStmt> parseFunctionCallStmt(std::shared_ptr<Stmt>);
    std::shared_ptr<Stmt> parseIdentifierStmt();
    std::vector<std::shared_ptr<Stmt>> parseBlock();
    std::shared_ptr<Stmt> parseExpressionUntil(TokenType);
    std::shared_ptr<Stmt> parseExpression();
    std::shared_ptr<Stmt> parseEquality();
    std::shared_ptr<Stmt> parseComparison();
    std::shared_ptr<Stmt> parseTerm();
    std::shared_ptr<Stmt> parseFactor();
    // std::shared_ptr<Stmt> parseUnary();
    std::shared_ptr<Stmt> parsePrimary();
};

#endif