#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateIdentifier(const std::shared_ptr<IdentifierStmt>& identifierStmt){
    VarInfo* var = lookupVariable(identifierStmt->name);
    if(!var) { throw std::runtime_error("Variable not found"); }
    
    if(!var->structTypeName.empty()) {
        // variabel struct: kembalikan pointer langsung, jangan load
        std::cout << "ini generate struct\n";
        return var->alloc;
    }

    switch(var->dataType) {
        case LiteralType::INT_32:
            return builder.CreateLoad(builder.getInt32Ty(), var->alloc, identifierStmt->name);
        case LiteralType::INT_64:
            return builder.CreateLoad(builder.getInt64Ty(), var->alloc, identifierStmt->name);
        // case LiteralType::FLOAT:
        //     return builder.CreateLoad(builder.getDoubleTy(), var->alloc, identifierStmt->name);    
        case LiteralType::DOUBLE:
            return builder.CreateLoad(builder.getDoubleTy(), var->alloc, identifierStmt->name);
        case LiteralType::BOOL:
            return builder.CreateLoad(builder.getInt1Ty(), var->alloc, identifierStmt->name);
        case LiteralType::STRING:
            return builder.CreateLoad(builder.getInt8PtrTy(), var->alloc, identifierStmt->name);
    }
    return nullptr;
}