#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateMemberAccess(const std::shared_ptr<MemberAccessExpr>& memberStmt){
    auto ident = std::static_pointer_cast<IdentifierStmt>(memberStmt->object);
    VarInfo* varInfo = lookupVariable(ident->name);
    if(!varInfo) throw std::runtime_error("Variable not found: " + ident->name);
    if(varInfo->structTypeName.empty() || varInfo->isArray)
        throw std::runtime_error("Variable is not a struct: " + ident->name);
    
    StructInfo& structInfo = StructTypes[varInfo->structTypeName];
    auto itField = structInfo.fieldIndices.find(memberStmt->memberName);
    if(itField == structInfo.fieldIndices.end())
        throw std::runtime_error("Unknown field: " + memberStmt->memberName);
    
    unsigned fieldIndex = itField->second;
    llvm::Value* fieldPtr = builder.CreateStructGEP(structInfo.type, varInfo->alloc, fieldIndex, memberStmt->memberName + "_ptr");
    
    // Jika field primitif, load nilainya untuk print / operasi
    llvm::Value* result = nullptr;
    llvm::Type* elemTy = fieldPtr->getType()->getPointerElementType();
    if(elemTy->isStructTy()){
        result = fieldPtr; // struct → kembalikan pointer
    } else {
        result = builder.CreateLoad(elemTy, fieldPtr, memberStmt->memberName);
    }
    
    return result;
}