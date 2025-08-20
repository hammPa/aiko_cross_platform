#include "../../include/CodeGen.hpp"

void CodeGen::handleFor(const std::shared_ptr<ForStmt>& forStmt){
    std::string name = forStmt->var_name;
    llvm::Value *start = generateExpression(forStmt->start);
    llvm::Value *end = generateExpression(forStmt->end);
    llvm::Value* one = llvm::ConstantInt::get(context, llvm::APInt(32, 1));
    llvm::Value* endMinusOne = builder.CreateSub(end, one, "end_minus_1");        
    llvm::Value *step = generateExpression(forStmt->step);
    
    // alokasi index
    llvm::AllocaInst *allocIndex = createEntryBlockAlloca(mainFun, name, builder.getInt32Ty());
    builder.CreateStore(start, allocIndex);
    
    // basic block
    llvm::BasicBlock *loopCondBB = llvm::BasicBlock::Create(context, "loop_cond", mainFun);
    llvm::BasicBlock *loopBodyBB = llvm::BasicBlock::Create(context, "loop_body", mainFun);
    llvm::BasicBlock *loopIncBB  = llvm::BasicBlock::Create(context, "loop_inc", mainFun);
    llvm::BasicBlock *loopEndBB  = llvm::BasicBlock::Create(context, "loop_end", mainFun);
    
    BreakTargets.push_back(loopEndBB);
    ContinueTargets.push_back(loopIncBB);    

    // branch dari current ke condition
    builder.CreateBr(loopCondBB);
    
    // block kondisi
    builder.SetInsertPoint(loopCondBB);
    llvm::Value *currentIndex = builder.CreateLoad(builder.getInt32Ty(), allocIndex, name);
    llvm::Value *cond = builder.CreateICmpSLE(currentIndex, endMinusOne, "loopcond"); // end -1
    builder.CreateCondBr(cond, loopBodyBB, loopEndBB);
    
    // body
    builder.SetInsertPoint(loopBodyBB);
    // generate block statements
    std::shared_ptr<ProgramStmt> progStmt = std::make_shared<ProgramStmt>(forStmt->block);
    
    
    
    enterScope();
    VariablesStack.back()[name] = { allocIndex, LiteralType::INT_32 }; // nanti bisa diganti selain 32
    for(const auto& stmt : progStmt->statements) {
        generateStatement(stmt);  // reuse statement handler
    }
    exitScope();
    // for(auto &[k,v] : VariablesStack.back()) {
    //     std::cout << "Variable in scope: " << k << std::endl;
    // }
    
    builder.CreateBr(loopIncBB);
    
    // Increment block
    builder.SetInsertPoint(loopIncBB);
    llvm::Value *currentIndexInc = builder.CreateLoad(builder.getInt32Ty(), allocIndex, name);
    llvm::Value *nextIndex = builder.CreateAdd(currentIndexInc, step, "nexti");
    builder.CreateStore(nextIndex, allocIndex);
    builder.CreateBr(loopCondBB);         // kembali ke condition
    
    // End block
    builder.SetInsertPoint(loopEndBB);
    BreakTargets.pop_back();
    ContinueTargets.pop_back();
}