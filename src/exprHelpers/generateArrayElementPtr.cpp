#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateArrayElementPtr(VarInfo* var, llvm::Value* idxVal){
    // runtime check: idx >= size ?
    llvm::Value* maxIndex = llvm::ConstantInt::get(builder.getInt32Ty(), var->size);
    llvm::Value* cond = builder.CreateICmpUGE(idxVal, maxIndex, "oobcheck");

    llvm::Function* parentFunc = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* errorBlock = llvm::BasicBlock::Create(context, "oob_error", parentFunc);
    llvm::BasicBlock* continueBlock = llvm::BasicBlock::Create(context, "oob_continue", parentFunc);

    builder.CreateCondBr(cond, errorBlock, continueBlock);

    // error block
    builder.SetInsertPoint(errorBlock);
    builder.CreateCall(module->getOrInsertFunction(
        "puts", llvm::FunctionType::get(builder.getInt32Ty(), builder.getInt8PtrTy(), true)
    ), builder.CreateGlobalStringPtr("Runtime Error: Array index out of bounds"));

    llvm::FunctionCallee exitFunc = module->getOrInsertFunction(
        "exit", llvm::FunctionType::get(builder.getVoidTy(), {builder.getInt32Ty()}, false)
    );
    builder.CreateCall(exitFunc, { llvm::ConstantInt::get(builder.getInt32Ty(), 1) });
    builder.CreateUnreachable();

    // continue block
    builder.SetInsertPoint(continueBlock);

    // GEP: ambil pointer ke arr[idx]
    std::vector<llvm::Value*> indices;
    indices.push_back(llvm::ConstantInt::get(builder.getInt32Ty(), 0));
    indices.push_back(idxVal);

    return builder.CreateGEP(var->alloc->getAllocatedType(), var->alloc, indices);
}
