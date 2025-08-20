#include "../../include/CodeGen.hpp"

void CodeGen::handlePrintArray(const VarInfo* varInfo) {
    auto arrayTy = llvm::cast<llvm::ArrayType>(varInfo->alloc->getAllocatedType());
    size_t arraySize = arrayTy->getNumElements();

    genPrintString(getBracketString(true), false);
    for (size_t i = 0; i < arraySize; i++) {
        llvm::Value* index = llvm::ConstantInt::get(context, llvm::APInt(32, i));
        std::vector<llvm::Value*> indices = {
            llvm::ConstantInt::get(context, llvm::APInt(32, 0)),
            index
        };

        llvm::Value* elPtr = builder.CreateGEP(varInfo->alloc->getAllocatedType(), varInfo->alloc, indices);
        llvm::Value* el = builder.CreateLoad(elPtr->getType()->getPointerElementType(), elPtr);

        if (varInfo->dataType == LiteralType::INT_32)
            genPrintInt(el, false);
        else if (varInfo->dataType == LiteralType::INT_64)
            genPrintInt(el, false);
        // else if (varInfo->dataType == LiteralType::FLOAT)
        //     genPrintDouble(el, false);
        else if (varInfo->dataType == LiteralType::DOUBLE)
            genPrintDouble(el, false);

        if (i != arraySize - 1)
        genPrintString(builder.CreateGlobalStringPtr(", "), false);
    }
    genPrintString(getBracketString(false), true);
}