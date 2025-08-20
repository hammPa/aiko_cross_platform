#include "../../include/CodeGen.hpp"

void CodeGen::handleStructDecl(const std::shared_ptr<StructStmt>& structStmt){
    // cek kalau struct sudah ada
    auto it = StructTypes.find(structStmt->name);
    if(it != StructTypes.end()){
        throw std::runtime_error("Struct already defined: " + structStmt->name);
    }

    std::vector<llvm::Type*> fieldTypes;
    std::unordered_map<std::string, unsigned> fieldIndices;

    for(size_t i = 0; i < structStmt->fields.size(); ++i){
        fieldTypes.push_back(literalTypeToLLVM(structStmt->fields[i]->type));
        fieldIndices[structStmt->fields[i]->name] = i;
    }

    llvm::StructType* structType = llvm::StructType::create(context, fieldTypes, structStmt->name);

    StructInfo info;
    info.type = structType;
    info.fieldIndices = std::move(fieldIndices);

    StructTypes[structStmt->name] = std::move(info);
}