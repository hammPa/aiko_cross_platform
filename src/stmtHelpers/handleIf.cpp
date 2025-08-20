#include "../../include/CodeGen.hpp"

void CodeGen::handleIf(const std::shared_ptr<IfStmt>& ifStmt){
    llvm::Function* mainFun = builder.GetInsertBlock()->getParent();
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(context, "if_merge", mainFun);
    
    // --- THEN block ---
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(context, "then", mainFun);
    llvm::BasicBlock* nextCondBB = nullptr;
    
    if (!ifStmt->elifs.empty()) {
        nextCondBB = llvm::BasicBlock::Create(context, "elif0_cond", mainFun);
    } else if (!ifStmt->else_block.empty()) {
        nextCondBB = llvm::BasicBlock::Create(context, "else_block", mainFun);
    } else {
        nextCondBB = mergeBB;
    }
    
    // --- Kondisi IF ---
    llvm::Value* condVal = generateExpression(ifStmt->condition);
    if (!condVal) {
        llvm::errs() << "Error: if condition returned null!\n";
        condVal = llvm::ConstantInt::getFalse(context); // fallback aman
    }
    if (!condVal->getType()->isIntegerTy(1)) {
        condVal = builder.CreateICmpNE(condVal, builder.getInt32(0));
    }
    
    builder.CreateCondBr(condVal, thenBB, nextCondBB);
    
    // --- THEN block ---
    enterScope();
    builder.SetInsertPoint(thenBB);
    for (auto& stmt : ifStmt->then_block)
        generateStatement(stmt);
    exitScope();    
    builder.CreateBr(mergeBB);
    
    // --- ELIF ---
    llvm::BasicBlock* prevCondBB = nextCondBB;
    for (size_t i = 0; i < ifStmt->elifs.size(); ++i) {
        auto& elif = ifStmt->elifs[i];
    
        llvm::BasicBlock* elifBodyBB = llvm::BasicBlock::Create(context, "elif_body_" + std::to_string(i), mainFun);
        llvm::BasicBlock* nextElifCondBB = nullptr;
    
        if (i + 1 < ifStmt->elifs.size()) {
            nextElifCondBB = llvm::BasicBlock::Create(context, "elif" + std::to_string(i+1) + "_cond", mainFun);
        } else if (!ifStmt->else_block.empty()) {
            nextElifCondBB = llvm::BasicBlock::Create(context, "else_block", mainFun);
        } else {
            nextElifCondBB = mergeBB;
        }
    
        // Kondisi ELIF
        builder.SetInsertPoint(prevCondBB);
        llvm::Value* elifCondVal = generateExpression(elif->condition);
        if (!elifCondVal) {
            llvm::errs() << "Error: elif condition returned null!\n";
            elifCondVal = llvm::ConstantInt::getFalse(context);
        }
        if (!elifCondVal->getType()->isIntegerTy(1)) {
            elifCondVal = builder.CreateICmpNE(elifCondVal, builder.getInt32(0));
        }
    
        builder.CreateCondBr(elifCondVal, elifBodyBB, nextElifCondBB);
    
        // Body ELIF
        enterScope();
        builder.SetInsertPoint(elifBodyBB);
        for (auto& stmt : elif->block)
            generateStatement(stmt);
        exitScope();
        builder.CreateBr(mergeBB);
    
        prevCondBB = nextElifCondBB;
    }
    
    // --- ELSE ---
    if (!ifStmt->else_block.empty()) {
        llvm::BasicBlock* elseBB = prevCondBB;
        enterScope();
        builder.SetInsertPoint(elseBB);
        for (auto& stmt : ifStmt->else_block)
            generateStatement(stmt);
        exitScope();
        builder.CreateBr(mergeBB);
    }
    
    // --- Merge ---
    builder.SetInsertPoint(mergeBB);
}