#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateArrayAccess(const std::shared_ptr<ArrayAccessStmt>& accessStmt){
    VarInfo *var = lookupVariable(accessStmt->array_name);
    if (!var || !var->isArray) {
        throw std::runtime_error("Variable is not an array");
    }
    
    llvm::Value* idxVal = generateExpression(accessStmt->index);
    llvm::Value* elemPtr = generateArrayElementPtr(var, idxVal);
    
    // load dari elemPtr karena ini expression
    return builder.CreateLoad(
        elemPtr->getType()->getPointerElementType(),
        elemPtr,
        "array_load"
    );
}
