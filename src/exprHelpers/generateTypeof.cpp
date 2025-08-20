#include "../../include/CodeGen.hpp"

// Helper mapping LiteralType ke string
static const std::unordered_map<LiteralType, std::string> literalTypeNames = {
    {LiteralType::INT_32, "i32"},
    {LiteralType::INT_64, "i64"},
    {LiteralType::DOUBLE, "double"},
    {LiteralType::BOOL, "bool"},
    {LiteralType::STRING, "str"},
    {LiteralType::UNKNOWN, "unknown"}
};

llvm::Value* CodeGen::generateTypeof(const std::shared_ptr<TypeofStmt>& typeOfStmtObj){
    auto expr = typeOfStmtObj->expression;
    LiteralType type = LiteralType::UNKNOWN;

    if(expr->type == StmtType::Identifier) {
        auto name = std::static_pointer_cast<IdentifierStmt>(expr)->name;
        auto varInfo = lookupVariable(name);
        if(!varInfo) throw std::runtime_error("Variable not found: " + name);
        type = varInfo->dataType;
    }
    else if(expr->type == StmtType::Literal) {
        type = std::static_pointer_cast<LiteralStmt>(expr)->dataType;
    }
    else {
        // fallback, bisa generate value
        llvm::Value* val = generateExpression(expr);
        type = inferLiteralTypeFromLLVM(val->getType());
        if (type == LiteralType::UNKNOWN)
            throw std::runtime_error("Unknown type in typeof");
    }

    // return string LLVM sesuai tipe
    return builder.CreateGlobalStringPtr(literalTypeNames.at(type));
}
