#include "../../include/CodeGen.hpp"

// llvm::Value* CodeGen::generateBinary(const std::shared_ptr<BinaryOpStmt>& binOp){
//     llvm::Value* left = generateExpression(binOp->left);
//     llvm::Value* right = generateExpression(binOp->right);

//     // Asumsikan INT untuk sekarang
//     if(binOp->op == "+")
//         return builder.CreateAdd(left, right, "addtmp");
//     else if(binOp->op == "-")
//         return builder.CreateSub(left, right, "subtmp");
//     else if(binOp->op == "*")
//         return builder.CreateMul(left, right, "multmp");
//     else if(binOp->op == "/")
//         return builder.CreateSDiv(left, right, "divtmp");
//     else if(binOp->op == "%")
//         return builder.CreateSRem(left, right, "modtmp");
    
//     // --- Perbandingan integer ---
    // if(binOp->op == "==") return builder.CreateICmpEQ(left, right, "eqtmp");
    // if(binOp->op == "!=") return builder.CreateICmpNE(left, right, "netmp");
    // if(binOp->op == "<")  return builder.CreateICmpSLT(left, right, "lttmp");
    // if(binOp->op == "<=") return builder.CreateICmpSLE(left, right, "letmp");
    // if(binOp->op == ">")  return builder.CreateICmpSGT(left, right, "gttmp");
    // if(binOp->op == ">=") return builder.CreateICmpSGE(left, right, "getmp");

//     throw std::runtime_error("Binary operator tidak dikenali atau tipe tidak didukung: " + binOp->op);
// }


llvm::Value* CodeGen::generateBinary(const std::shared_ptr<BinaryOpStmt>& binOp) {
    llvm::Value* left = generateExpression(binOp->left);
    llvm::Value* right = generateExpression(binOp->right);


    // cek tipe float
    bool isFloat = left->getType()->isDoubleTy() || right->getType()->isDoubleTy();
    if(isFloat){
        if(!left->getType()->isDoubleTy()) left = builder.CreateSIToFP(left, llvm::Type::getDoubleTy(context), "leftfp");
        if(!right->getType()->isDoubleTy()) right = builder.CreateSIToFP(right, llvm::Type::getDoubleTy(context), "rightfp");
    }

    // handle operator
    if(binOp->op == "+") return isFloat ? builder.CreateFAdd(left, right, "faddtmp") : builder.CreateAdd(left, right, "addtmp");
    else if(binOp->op == "-") return isFloat ? builder.CreateFSub(left, right, "fsubtmp") : builder.CreateSub(left, right, "subtmp");
    else if(binOp->op == "*") return isFloat ? builder.CreateFMul(left, right, "fmultmp") : builder.CreateMul(left, right, "multtmp");
    else if(binOp->op == "/") return isFloat ? builder.CreateFDiv(left, right, "fdivtmp") : builder.CreateSDiv(left, right, "divtmp");
    else if(binOp->op == "%") {
        if(isFloat){
            llvm::FunctionType* fmodType = llvm::FunctionType::get(
                llvm::Type::getDoubleTy(context),
                {llvm::Type::getDoubleTy(context), llvm::Type::getDoubleTy(context)},
                false);
            llvm::FunctionCallee fmodFunc = module->getOrInsertFunction("fmod", fmodType);
            return builder.CreateCall(fmodFunc, {left, right}, "fmodtmp");
        } else return builder.CreateSRem(left, right, "modtmp");
    }
    else if(binOp->op == "==") {
        // jika string (i8*), pakai strcmp
        if(left->getType()->isPointerTy() && left->getType()->getPointerElementType()->isIntegerTy(8))
            return generateStringEq(left, right);
        else return isFloat ? builder.CreateFCmpOEQ(left, right, "feqtmp") : builder.CreateICmpEQ(left, right, "eqtmp");
    }
    else if(binOp->op == "!=") {
        if(left->getType()->isPointerTy() && left->getType()->getPointerElementType()->isIntegerTy(8))
            return builder.CreateNot(generateStringEq(left, right), "strne");
        else return isFloat ? builder.CreateFCmpONE(left, right, "fnetmp") : builder.CreateICmpNE(left, right, "netmp");
    }
    else if(binOp->op == "<") return isFloat ? builder.CreateFCmpOLT(left, right, "flttmp") : builder.CreateICmpSLT(left, right, "lttmp");
    else if(binOp->op == "<=") return isFloat ? builder.CreateFCmpOLE(left, right, "fletmp") : builder.CreateICmpSLE(left, right, "letmp");
    else if(binOp->op == ">") return isFloat ? builder.CreateFCmpOGT(left, right, "fgttmp") : builder.CreateICmpSGT(left, right, "gttmp");
    else if(binOp->op == ">=") return isFloat ? builder.CreateFCmpOGE(left, right, "fgettmp") : builder.CreateICmpSGE(left, right, "getmp");

    throw std::runtime_error("Binary operator tidak dikenali: " + binOp->op);
}

// helper function untuk string comparison
llvm::Value* CodeGen::generateStringEq(llvm::Value* lhs, llvm::Value* rhs){
    auto strcmpFunc = module->getOrInsertFunction("strcmp",
        llvm::FunctionType::get(builder.getInt32Ty(),
                                {builder.getInt8PtrTy(), builder.getInt8PtrTy()},
                                false));
    llvm::Value* result = builder.CreateCall(strcmpFunc, {lhs, rhs}, "strcmptmp");
    return builder.CreateICmpEQ(result, llvm::ConstantInt::get(builder.getInt32Ty(), 0), "streqtmp");
}
