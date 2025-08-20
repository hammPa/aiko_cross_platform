#include "../../include/CodeGen.hpp"


llvm::Value* CodeGen::generateStructInit(const std::shared_ptr<StructExpr>& structInitStmt){
    auto it = StructTypes.find(structInitStmt->name);
    if(it == StructTypes.end())
        throw std::runtime_error("Unknown struct type: " + structInitStmt->name);
    
    StructInfo& structInfo = it->second;
    llvm::StructType* structType = structInfo.type;
    llvm::Function* currentFun = builder.GetInsertBlock()->getParent();
    
    llvm::AllocaInst* alloc = createEntryBlockAlloca(currentFun, structInitStmt->name + "_inst", structType, 1);
    
    for(size_t i=0;i<structInitStmt->fieldsValue.size();++i){
        auto& field = structInitStmt->fieldsValue[i];
        unsigned fieldIndex = structInfo.fieldIndices[field.first];
        llvm::Value* fieldPtr = builder.CreateStructGEP(structType, alloc, fieldIndex, field.first + "_ptr");
        llvm::Value* value = generateExpression(field.second);
        builder.CreateStore(value, fieldPtr);
    }
    
    VarInfo varInfo;
    varInfo.alloc = alloc;
    varInfo.dataType = LiteralType::STRUCT;
    varInfo.structTypeName = structInitStmt->name;
    varInfo.isArray = false;
    varInfo.size = 1;
    varInfo.staticType = true;
    
    // simpan ke stack dengan nama tipe struct sementara
    VariablesStack.back()[structInitStmt->name] = varInfo;
    return alloc;
}