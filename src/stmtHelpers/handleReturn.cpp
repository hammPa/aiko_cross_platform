#include "../../include/CodeGen.hpp"

void CodeGen::handleReturn(const std::shared_ptr<ReturnStmt>& ret){
    if (ret->value) {
        llvm::Value* retVal = generateExpression(ret->value);
        builder.CreateRet(retVal);
    } else {
        // kalau function bukan void → paksa return 0
        auto* func = builder.GetInsertBlock()->getParent();
        if (func->getReturnType()->isVoidTy()) {
            builder.CreateRetVoid();
        } else {
            builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
        }
    }
    return;
}