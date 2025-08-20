#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateFunctionCall(const std::shared_ptr<FunctionCallStmt>& callStmt){
    llvm::Function* calleeFunc = module->getFunction(callStmt->name);
    if(!calleeFunc) {
        throw std::runtime_error("Undefined function: " + callStmt->name);
    }
    
    std::vector<llvm::Value*> argsV;
    for(auto& arg : callStmt->args) {
        argsV.push_back(generateExpression(arg));
    }
    
    if(calleeFunc->getReturnType()->isVoidTy()) {
        return builder.CreateCall(calleeFunc, argsV); 
    } else {
        return builder.CreateCall(calleeFunc, argsV, "calltmp");
    }
}