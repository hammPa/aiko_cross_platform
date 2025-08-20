#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateArrayLiteral(const std::shared_ptr<ArrayLiteralStmt>& arrStmt){
    if(arrStmt->elements.empty()) throw std::runtime_error("Array tidak boleh kosong");
    // ambil elemen pertama
    auto firstEl = arrStmt->elements[0];
    llvm::Value *firstVal = generateExpression(firstEl);
    llvm::Type *elType = firstVal->getType();
    
    // validasi semua elemen tipenya sama
    for(size_t i = 0; i < arrStmt->elements.size(); i++){
        llvm::Value *val = generateExpression(arrStmt->elements[i]);
        if(val->getType() != elType){
            throw std::runtime_error("Tipe data tidak sama");
        }
    }
    
    // buat array type
    llvm::ArrayType *arrType = llvm::ArrayType::get(elType, arrStmt->elements.size());
    
    // alokasi alamat di stack
    llvm::Function *currentFunc = builder.GetInsertBlock()->getParent();
    llvm::AllocaInst *allocArr = createEntryBlockAlloca(currentFunc, "arrLit", arrType);
    
    // simpan elemen
    for(size_t i = 0; i < arrStmt->elements.size(); i++){
        llvm::Value *zero = llvm::ConstantInt::get(builder.getInt32Ty(), 0);
        llvm::Value *idx = llvm::ConstantInt::get(builder.getInt32Ty(), i);
        
        llvm::Value *elPtr = builder.CreateGEP(arrType, allocArr, {zero, idx});
        llvm::Value *elVal = generateExpression(arrStmt->elements[i]);
    
        builder.CreateStore(elVal, elPtr);
    }
    
    return allocArr;
}