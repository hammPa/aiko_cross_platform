#include "../../include/CodeGen.hpp"

void CodeGen::handleVarDeclaration(const std::shared_ptr<VarDeclStmt>& varDeclStmt){
    llvm::AllocaInst* allocVar = nullptr;
    LiteralType varType = varDeclStmt->varType;
    bool isArray = false;
    size_t size = 1;

    llvm::Function *currentFunc = builder.GetInsertBlock()->getParent();

    bool isStaticType = varDeclStmt->hasExplicit;

   // Kalau deklarasi tanpa inisialisasi
   if (!varDeclStmt->initializer) {
        if (!isStaticType) {
            varType = LiteralType::INT_32; // default
        }
        llvm::Type* llvmTy = literalTypeToLLVM(varType);
        allocVar = createEntryBlockAlloca(currentFunc, varDeclStmt->name, llvmTy);

        // nilai default (misalnya 0)
        builder.CreateStore(defaultValueForType(varType), allocVar);
        VariablesStack.back()[varDeclStmt->name] = { allocVar, varType, isArray, size, isStaticType };
        return;
    }


    llvm::Value* initVal = generateExpression(varDeclStmt->initializer);
    if(!initVal) { 
        std::runtime_error("Error generating initializer\n"); 
        return; 
    }

    // alokasi memori
    // Pilih tipe Alloca sesuai ekspresi
    if(varDeclStmt->initializer->type == StmtType::StructInit){
        auto structInitStmt = std::static_pointer_cast<StructExpr>(varDeclStmt->initializer);
    
        // Ambil LLVM StructType dari StructTypes
        auto it = StructTypes.find(structInitStmt->name);
        if(it == StructTypes.end())
            throw std::runtime_error("Unknown struct type: " + structInitStmt->name);
    
        llvm::StructType* structType = it->second.type;
    
        // alokasi AllocaInst untuk struct
        allocVar = createEntryBlockAlloca(currentFunc, varDeclStmt->name, structType, 1);
    
        // isi field jika ada nilai awal
        for(size_t i=0; i<structInitStmt->fieldsValue.size(); ++i){
            auto& field = structInitStmt->fieldsValue[i];
            unsigned fieldIndex = it->second.fieldIndices[field.first];
            llvm::Value* fieldPtr = builder.CreateStructGEP(structType, allocVar, fieldIndex, field.first + "_ptr");
            llvm::Value* value = generateExpression(field.second);
            builder.CreateStore(value, fieldPtr);
        }
    
        // simpan ke stack dengan pointer yang benar
        VariablesStack.back()[varDeclStmt->name] = {
            allocVar,       // penting! harus pointer AllocaInst struct
            LiteralType::STRUCT,
            false,          // isArray
            1,              // size
            isStaticType,
            structInitStmt->name // nama tipe struct
        };
        return;
    }    
    else if(varDeclStmt->initializer->type == StmtType::ArrayLiteral){
        // std::cout << "skrg variabel sudah menunjuk alamat pertama\n";
        auto arrStmt = std::static_pointer_cast<ArrayLiteralStmt>(varDeclStmt->initializer);

        llvm::Value *firstVal = generateExpression(arrStmt->elements[0]);
        varType = inferLiteralTypeFromLLVM(firstVal->getType());
        allocVar = static_cast<llvm::AllocaInst*>(initVal);

        isArray = true;
        size = sizeofArray(arrStmt);
    }
    else if(varDeclStmt->initializer->type == StmtType::Literal) {
        auto literalStmt = std::static_pointer_cast<LiteralStmt>(varDeclStmt->initializer);
        llvm::Type *llvmTy = literalTypeToLLVM(literalStmt->dataType);

        varType = literalStmt->dataType;
        allocVar = createEntryBlockAlloca(currentFunc, varDeclStmt->name, llvmTy);
    }
    else if(varDeclStmt->initializer->type == StmtType::FunctionCall){ // menyimpan nilai return dari expression function
        auto callExpr = std::static_pointer_cast<FunctionCallStmt>(varDeclStmt->initializer);
        llvm::Value* callVal = generateExpression(callExpr);
        if (!callVal) throw std::runtime_error("Error generating function call initializer for variable " + varDeclStmt->name);
    
        llvm::Type* allocType = callVal->getType();
        allocVar = createEntryBlockAlloca(mainFun, varDeclStmt->name, allocType);
    
        builder.CreateStore(callVal, allocVar);
        varType = inferLiteralTypeFromLLVM(allocType);
        VariablesStack.back()[varDeclStmt->name] = { allocVar, varType, false, 1, isStaticType };
    }
    else if(varDeclStmt->initializer->type == StmtType::Input){
        // ambil tipe LLVM dari value yang sudah dikonversi
        llvm::Type* llvmType = initVal->getType();

        // alokasi variabel sesuai tipe target
        allocVar = createEntryBlockAlloca(currentFunc, varDeclStmt->name, llvmType);

        // simpan value hasil konversi ke variabel
        builder.CreateStore(initVal, allocVar);

        // set varType sesuai literal type
        varType = inferLiteralTypeFromLLVM(llvmType);
    }    
    else { 
        // asumsi binary op → INT (bisa dikembangkan sesuai tipe ekspresi)
        llvm::Type *allocType = initVal->getType();
        allocVar = createEntryBlockAlloca(currentFunc, varDeclStmt->name, allocType);
        // infer tipe literal dari LLVM Type
        varType = inferLiteralTypeFromLLVM(allocType);
    }

    // masukkan nilai ke alamat
    if(varDeclStmt->initializer->type == StmtType::Input){
        // store sudah dilakukan saat alokasi Input
        // tidak perlu lagi store di sini
    }    
    else if (varDeclStmt->initializer->type != StmtType::ArrayLiteral) {
        // Kalau array, gak perlu store, karena sudah pointer
        builder.CreateStore(initVal, allocVar);
    }
    VariablesStack.back()[varDeclStmt->name] = { allocVar, varType, isArray, size, isStaticType };
    // std::cout << "Variabel yang di masukkan array ?: " << VariablesStack.back()[varDeclStmt->name].isArray << '\n';
}