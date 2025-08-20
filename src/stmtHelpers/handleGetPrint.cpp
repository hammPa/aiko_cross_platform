#include "../../include/CodeGen.hpp"


llvm::Function* CodeGen::getPrintf(llvm::Module *m){
    if(printFun) return printFun;

    std::vector<llvm::Type*> printfArgs;
    printfArgs.push_back(builder.getInt8PtrTy());

    llvm::FunctionType *printfType = llvm::FunctionType::get(
        builder.getInt32Ty(), printfArgs, true
    );
    
    printFun = llvm::Function::Create(
        printfType,
        llvm::Function::ExternalLinkage,
        "printf",
        m
    );

    printFun->setCallingConv(llvm::CallingConv::C);
    return printFun;
}

void CodeGen::genPrintString(llvm::Value *strVal, bool enter){
    llvm::Function *printFun = getPrintf(module.get());
    llvm::Value *fmt;
    if(enter) fmt = builder.CreateGlobalStringPtr("%s\n");
    else fmt = builder.CreateGlobalStringPtr("%s");
    builder.CreateCall(printFun, {fmt, strVal});
}
    
void CodeGen::genPrintInt(llvm::Value *intVal, bool enter) {
    llvm::Function *printFun = getPrintf(module.get());
    llvm::Type* valType = intVal->getType();
    llvm::Value *fmt;

    if(valType->isIntegerTy(64)) {
        // 64-bit integer → %ld
        fmt = builder.CreateGlobalStringPtr(enter ? "%ld\n" : "%ld");
    } else {
        // default 32-bit integer → %d
        fmt = builder.CreateGlobalStringPtr(enter ? "%d\n" : "%d");
    }

    builder.CreateCall(printFun, {fmt, intVal});
}

void CodeGen::genPrintDouble(llvm::Value *doubleVal, bool enter) {
    llvm::Function *printFun = getPrintf(module.get());
    
    if (doubleVal->getType()->isFloatTy()) {
        doubleVal = builder.CreateFPExt(doubleVal, builder.getDoubleTy(), "tmpdbl");
    } else if (doubleVal->getType()->isDoubleTy()) {
        // ok
    } else {
        // Kalau sampai bukan float/double, kasih error biar kelihatan cepat
        throw std::runtime_error("genPrintDouble: expected float/double");
    }

    llvm::Value *fmt;
    if(enter) fmt = builder.CreateGlobalStringPtr("%f\n");
    else fmt = builder.CreateGlobalStringPtr("%f");
    builder.CreateCall(printFun, {fmt, doubleVal});
}