#include "../../include/CodeGen.hpp"



void CodeGen::handlePrint(const std::shared_ptr<PrintStmt>& printStmtObj, bool enter){
    llvm::Value* val = nullptr;
    LiteralType type;
    
    if (printStmtObj->expression->type == StmtType::Identifier) {
        auto name = std::static_pointer_cast<IdentifierStmt>(printStmtObj->expression)->name;
        auto varInfo = lookupVariable(name);
        if (!varInfo)  throw std::runtime_error("Variable not found: " + name);
    
        if(varInfo->isArray){ // pakai print terpisah bukan print dibawah
            handlePrintArray(varInfo);
            return;
        }
        else { // selain array, printnya yang di bawah ini
            val = generateExpression(printStmtObj->expression);
            type = varInfo->dataType;
        }
    }
    else if (printStmtObj->expression->type == StmtType::Literal) {
        auto lit = std::static_pointer_cast<LiteralStmt>(printStmtObj->expression);
        val = generateExpression(printStmtObj->expression);
        type = lit->dataType;
    }
    else if(printStmtObj->expression->type == StmtType::ArrayAccess){
        auto arrAccess = std::static_pointer_cast<ArrayAccessStmt>(printStmtObj->expression);
        std::string name = arrAccess->array_name;
        VarInfo *varInfo = lookupVariable(name);
        if(!varInfo) throw std::runtime_error("Variable not found");
        
        llvm::Value *index = generateExpression(arrAccess->index);
        // if (auto constIntVal = llvm::dyn_cast<llvm::ConstantInt>(index)) {
        //     int64_t constIndex = constIntVal->getSExtValue();
        //     if (constIndex >= varInfo->size) {
        //         throw std::runtime_error("Error: out of the bounds");
        //     }
        // }
        
        
        // kalau index adalah pointer ke variabel, load nilainya
        if (index->getType()->isPointerTy()) {
            index = builder.CreateLoad(index->getType()->getPointerElementType(), index);
        }
        
        // cast ke i32
        if (!index->getType()->isIntegerTy(32)) {
            index = builder.CreateIntCast(index, llvm::Type::getInt32Ty(context), true);
        }
        
        // alamat elemen
        std::vector<llvm::Value*> indices = {
            llvm::ConstantInt::get(context, llvm::APInt(32, 0)),
            index
        };

        llvm::Value* elementPtr = builder.CreateGEP(varInfo->alloc->getAllocatedType(), varInfo->alloc, indices);
        
        // load nilainya
        val = builder.CreateLoad(elementPtr->getType()->getPointerElementType(), elementPtr);
        type = varInfo->dataType;
        
    }
    else {
        val = generateExpression(printStmtObj->expression);
        val->getType()->print(llvm::errs());
        llvm::errs() << "\n";
        llvm::Type* llvmType = val->getType();
        type = inferLiteralTypeFromLLVM(llvmType);
    }

    switch (type) {
        case LiteralType::INT_32: genPrintInt(val);    break;
        case LiteralType::INT_64: genPrintInt(val);    break;
        case LiteralType::DOUBLE: genPrintDouble(val); break;
        case LiteralType::BOOL: genPrintInt(val);    break;
        case LiteralType::STRING: genPrintString(val, enter); break;
        default: throw std::runtime_error("Unsupported type for print");
    }
}
