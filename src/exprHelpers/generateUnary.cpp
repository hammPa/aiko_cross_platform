#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateUnary(const std::shared_ptr<UnaryOpStmt>& unary){
    llvm::Value *operandVal = generateExpression(unary->operand);
    llvm::Type* operandType = operandVal->getType();
    // if(unary->op == "+") {
        // unary plus, kembalikan apa adanya
        // return operandVal;
    // } 
    if(unary->op == "-") {
        if(operandType->isIntegerTy())
            return builder.CreateNeg(operandVal, "negtmp");
        else if(operandType->isFloatingPointTy()) // berkoma baik float atau double
            return builder.CreateFNeg(operandVal, "fnegtmp");
        else {
            throw std::runtime_error("Unary minus hanya untuk INT/FLOAT\n");
            return nullptr;
        }
    } 
    else if(unary->op == "!") {
        if(operandType->isIntegerTy(1))  // boolean
            return builder.CreateNot(operandVal, "nottmp");
        else {
            throw std::runtime_error("Unary NOT hanya untuk BOOL\n");
            return nullptr;
        }
    } 
    else {
        throw std::runtime_error("Unary operator tidak dikenali: " + unary->op + "\n");
        return nullptr;
    }
}