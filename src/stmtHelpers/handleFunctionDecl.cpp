#include "../../include/CodeGen.hpp"

void CodeGen::handleFunctionDecl(const std::shared_ptr<FunctionDeclStmt>& funcDecl){    
    // buat tipe parameter
    std::vector<llvm::Type*> paramTypes;
    for (size_t i = 0; i < funcDecl->params.size(); i++) {
        auto varDecl = std::static_pointer_cast<VarDeclStmt>(funcDecl->params[i]);
        llvm::Type* argTy;
        if (varDecl->hasExplicit) {
            argTy = literalTypeToLLVM(varDecl->varType);
        }
        else { // default, diubah nanti
            argTy = builder.getInt32Ty();
        }
        paramTypes.push_back(argTy);
    }

    // buat fun
    llvm::Type* retType = builder.getInt32Ty();
    llvm::FunctionType *funcType = llvm::FunctionType::get(retType, paramTypes, false);
    llvm::Function* function = llvm::Function::Create(
        funcType,
        llvm::Function::ExternalLinkage,
        funcDecl->name,
        module.get()
    );

    // entry block
    llvm::BasicBlock* entry = llvm::BasicBlock::Create(context, "entry", function);
    auto* oldBlock = builder.GetInsertBlock(); // simpan posisi builder lama untuk kembali ke sebelumnya
    builder.SetInsertPoint(entry);

    VariablesStack.emplace_back();
    // mapping args ke allocas
    auto argIter = function->arg_begin();
    for (size_t i = 0; i < funcDecl->params.size(); i++, argIter++) {
        auto varDecl = std::static_pointer_cast<VarDeclStmt>(funcDecl->params[i]);
        llvm::Type* argTy;
        LiteralType varType;
        if (varDecl->hasExplicit) {
            argTy = literalTypeToLLVM(varDecl->varType);
            varType = varDecl->varType;
        } else {
            argTy = builder.getInt32Ty(); // default
            varType = LiteralType::INT_32; // default agar variable bisa dikenali
        }

        
        const std::string& paramName = varDecl->name;
        argIter->setName(paramName);

        llvm::AllocaInst* alloc = createEntryBlockAlloca(function, paramName, argTy);
        builder.CreateStore(argIter, alloc);

        VariablesStack.back()[paramName] = { alloc, varType, false, 1 };
    }


    // inferred return type
    // llvm::Type* inferredRetType = builder.getInt32Ty(); // default fallback
    // for (auto& s : funcDecl->body) {
    //     if (s->type == StmtType::Return) {
    //         auto retStmt = std::static_pointer_cast<ReturnStmt>(s);
    //         llvm::Value* retVal = generateExpression(retStmt->value);
    //         inferredRetType = retVal->getType();
    //         break; // ambil tipe dari return pertama
    //     }
    // }

    // // --- update function type kalau perlu ---
    // if (retType != builder.getInt32Ty()) {
    //     llvm::FunctionType* newFuncType = llvm::FunctionType::get(retType, paramTypes, false);
    //     function->setFunctionType(newFuncType); // LLVM 16 support update? jika tidak, abaikan
    // }

    std::cout << "inferred\n";

    for (auto& s : funcDecl->body) generateStatement(s);

    // kalau gak ada return, tambahin default
    if (!entry->getTerminator()) builder.CreateRet(llvm::ConstantInt::get(builder.getInt32Ty(), 0));

    VariablesStack.pop_back();
    if (oldBlock) { // balikin insert point ke block sebelumnya (contoh: main)
        builder.SetInsertPoint(oldBlock);
    }
}