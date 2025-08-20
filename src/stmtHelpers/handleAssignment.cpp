#include "../../include/CodeGen.hpp"

void CodeGen::handleAssignment(const std::shared_ptr<AssignmentStmt>& assignmentStmt){
    std::string name = assignmentStmt->name;
    llvm::Value* newVal = generateExpression(assignmentStmt->value);
    if(!newVal) std::runtime_error("Error generating assignment value"); 
    
    // ambil info variabel lama
    VarInfo* oldVar = lookupVariable(name);
    if(!oldVar) std::runtime_error("Variable not found");
    LiteralType oldType = oldVar->dataType;
    
    // tentukan tipe value baru
    LiteralType newType;
    switch(assignmentStmt->value->type){
        case StmtType::Literal: 
            newType = std::static_pointer_cast<LiteralStmt>(assignmentStmt->value)->dataType;
            break;
        case StmtType::Identifier:
            newType = VariablesStack.back()[std::static_pointer_cast<IdentifierStmt>(assignmentStmt->value)->name].dataType;
            break;
        case StmtType::UnaryOp:
        case StmtType::BinaryOp:
            newType = LiteralType::INT_32; // asumsi default INT_32
            break;
        default:
            newType = LiteralType::INT_32;
            break;
    }

    // assignment ke elemen array
    if(assignmentStmt->index){
        if(newType != oldVar->dataType) std::runtime_error("Error: cannot assign value with different type to array element");

        llvm::Value *idxVal = generateExpression(assignmentStmt->index);
        llvm::Value* elemPtr = generateArrayElementPtr(oldVar, idxVal);

        builder.CreateStore(newVal, elemPtr);
        return;
    }
    else if(oldVar->isArray) std::runtime_error("Error: cannot assign single value to array directly. Use index.");

    // untuk variabel non array, jika tipe berubah, buat alloca baru
    if(newType != oldType){
        if(oldVar->staticType){
            throw std::runtime_error("Error: cannot assign value with difference type to static type element");
        }
        else {
            llvm::Type* llvmType = literalTypeToLLVM(newType);
        
            llvm::AllocaInst* newAlloc = createEntryBlockAlloca(
                builder.GetInsertBlock()->getParent(),
                name,
                llvmType
            );

            builder.CreateStore(newVal, newAlloc);
            VariablesStack.back()[name] = { newAlloc, newType, false, 1, false };
            return;
        }
    }
    
    // store value baru
    builder.CreateStore(newVal, VariablesStack.back()[name].alloc);
    
}