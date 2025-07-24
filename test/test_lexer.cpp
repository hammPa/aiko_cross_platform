#include <fstream>
#include <iostream>
#include <sstream>
#include "../src/Lexer.cpp"

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
    std::cout << "Jumlah token: " << tokens.size();

    for(const auto& token: tokens){
        std::cout << "type: " << tokenTypeToString(token.type) << ", value: " << token.value << "\n";
    }

    return 0;
}