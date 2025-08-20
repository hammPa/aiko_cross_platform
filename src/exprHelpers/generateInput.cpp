#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateInput(const std::shared_ptr<InputStmt>& inputStmt){
    // print prompt
    if(inputStmt->expr){ // jika ada literal di Input
        auto promptLiteral = std::make_shared<PrintStmt>(inputStmt->expr);
        handlePrint(promptLiteral, false);
    }

    // alokasi buffer untuk input string
    llvm::AllocaInst *buffer = createEntryBlockAlloca(mainFun, "input_buffer", builder.getInt8Ty(), 256);
    // panggil runtime function untuk input (misal getchar atau scanf)
    llvm::FunctionType* funcType = llvm::FunctionType::get(
        builder.getVoidTy(),
        {builder.getInt8PtrTy()},
        false
    );
    llvm::Function* inputFunc = module->getFunction("runtime_input");
    if (!inputFunc) {
        inputFunc = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            "runtime_input",
            module.get()
        );
    }
    builder.CreateCall(inputFunc, {buffer}); // buffer jadi tempat menyimpan input
    
    // konversi sesuai tipe
    if(inputStmt->dataType == "i32") {
        llvm::Function* atoiFunc = module->getFunction("atoi");
        if(!atoiFunc){
            llvm::FunctionType* atoiType = llvm::FunctionType::get(builder.getInt32Ty(), {builder.getInt8PtrTy()}, false);
            atoiFunc = llvm::Function::Create(atoiType, llvm::Function::ExternalLinkage, "atoi", module.get());
        }
        return builder.CreateCall(atoiFunc, {buffer}); // i32
    }
    else if(inputStmt->dataType == "i64") {
        llvm::Function* atoiFunc = module->getFunction("atoi");
        if(!atoiFunc){
            llvm::FunctionType* atoiType = llvm::FunctionType::get(builder.getInt64Ty(), {builder.getInt8PtrTy()}, false);
            atoiFunc = llvm::Function::Create(atoiType, llvm::Function::ExternalLinkage, "atoi", module.get());
        }
        return builder.CreateCall(atoiFunc, {buffer}); // i32
    }
    else if(inputStmt->dataType == "float") {
        llvm::Function* atofFunc = module->getFunction("atof");
        if(!atofFunc){
            llvm::FunctionType* atofType = llvm::FunctionType::get(builder.getFloatTy(), {builder.getInt8PtrTy()}, false);
            atofFunc = llvm::Function::Create(atofType, llvm::Function::ExternalLinkage, "atof", module.get());
        }
        return builder.CreateCall(atofFunc, {buffer}); // double
    }
    else if(inputStmt->dataType == "double") {
        llvm::Function* atofFunc = module->getFunction("atof");
        if(!atofFunc){
            llvm::FunctionType* atofType = llvm::FunctionType::get(builder.getDoubleTy(), {builder.getInt8PtrTy()}, false);
            atofFunc = llvm::Function::Create(atofType, llvm::Function::ExternalLinkage, "atof", module.get());
        }
        return builder.CreateCall(atofFunc, {buffer}); // double
    }
    else if(inputStmt->dataType == "bool") {
        llvm::Function* strcmpFunc = module->getFunction("strcmp");
        if(!strcmpFunc){
            llvm::FunctionType* strcmpType = llvm::FunctionType::get(builder.getInt32Ty(), {builder.getInt8PtrTy(), builder.getInt8PtrTy()}, false);
            strcmpFunc = llvm::Function::Create(strcmpType, llvm::Function::ExternalLinkage, "strcmp", module.get());
        }
        llvm::Value* cmp = builder.CreateCall(strcmpFunc, {buffer, builder.CreateGlobalStringPtr("0")});
        return builder.CreateICmpNE(cmp, builder.getInt32(0)); // i1
    }
    else { // string
        return buffer; // i8*
    }
}