#include <fstream>
#include <iostream>
#include <sstream>
#include "../include/Lexer.hpp"
#include "../include/Parser.hpp"
#include "../include/CodeGen.hpp"
#include <stdexcept>

#define RED "\033[31m"
#define RESET "\033[0m"

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    std::ostringstream buffer;

    if (!file.is_open()) {
        throw std::runtime_error("Tidak bisa membuka file.");
    }

    buffer << file.rdbuf();  // Membaca seluruh isi file
    return buffer.str();
}


int main(int argc, const char* argv[]){
    if (argc < 2) {
        std::cerr << "Gunakan: ./program <path_ke_file>\n";
        return 1;
    }

    std::string path = argv[1];
    std::string code = readFile(path);

    // std::cout << "Isi file:\n" << code << "\n";

    Lexer lexer = Lexer(code);
    std::vector<Token> tokens = lexer.tokenize();
    // std::cout << "Jumlah token: " << tokens.size();

    for(const auto& token: tokens){
        std::cout << "type: " << tokenTypeToString(token.type) << ", value: " << token.value << "\n";
    }
    std::shared_ptr<ProgramStmt> ast_tree;
    try {
        Parser parser = Parser(tokens);
        ast_tree = parser.parse();
        printStmt(ast_tree);
    }
    catch (const std::runtime_error& e) {
        std::cerr << RED << "[Parser Error] "  << RESET << e.what() << '\n';
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << RED << "[Unexpected Error] " << RESET << e.what() << '\n';
        return 1;
    }


    try {
        CodeGen codegen(ast_tree);
        codegen.run(true);
    }
    catch(const std::exception& e) {
        std::cerr << RED << "[Error] " << RESET << e.what() << '\n';
        return -1;
    }
    

    return 0;
}