#include "../include/CodeGen.hpp"
#include "../src/runtime/runtime.hpp"



CodeGen::CodeGen(const std::shared_ptr<ProgramStmt>& ast_tree)
    : builder(context), ast_tree(std::move(ast_tree)) {
    module = std::make_unique<llvm::Module>("aiko", context);
    makeMainFunction();
}



void CodeGen::generateProgram(const std::shared_ptr<ProgramStmt>& ast_tree){
    enterScope(); // push stack pertama
    for(const auto& stmt: ast_tree->statements) generateStatement(stmt);
    exitScope();
}



void CodeGen::generateStatement(const std::shared_ptr<Stmt>& stmt){
    // printStmt(stmt);
    // std::cout << "jumlah stack: " << VariablesStack.size() << '\n';
    if(stmt->type == StmtType::Print){
        auto printStmtObj = std::static_pointer_cast<PrintStmt>(stmt);
        handlePrint(printStmtObj);
    }
    else if(stmt->type == StmtType::VarDecl){
        auto varDeclStmt = std::static_pointer_cast<VarDeclStmt>(stmt);
        handleVarDeclaration(varDeclStmt);
    }
    else if(stmt->type == StmtType::Assignment){
        auto assignmentStmt = std::static_pointer_cast<AssignmentStmt>(stmt);
        handleAssignment(assignmentStmt);
    }
    else if(stmt->type == StmtType::If){
        auto ifStmt = std::static_pointer_cast<IfStmt>(stmt);
        handleIf(ifStmt);
    }
    else if(stmt->type == StmtType::For){
        auto forStmt = std::static_pointer_cast<ForStmt>(stmt);
        handleFor(forStmt);
    }
    else if(stmt->type == StmtType::Break){ // menangani double jump
        builder.CreateBr(BreakTargets.back());
        // buat block baru agar LLVM bisa generate instruksi setelah break
        llvm::BasicBlock *contBB = llvm::BasicBlock::Create(context, "after_break", mainFun);
        builder.SetInsertPoint(contBB);
    }
    else if(stmt->type == StmtType::Continue){
        builder.CreateBr(ContinueTargets.back());
        llvm::BasicBlock *contBB = llvm::BasicBlock::Create(context, "after_continue", mainFun);
        builder.SetInsertPoint(contBB);
    }
    else if (stmt->type == StmtType::FunctionDecl) {
        auto funcDecl = std::static_pointer_cast<FunctionDeclStmt>(stmt);
        handleFunctionDecl(funcDecl);
    }
    else if(stmt->type == StmtType::FunctionCall){ // function call langsung bukan expression
        auto callStmt = std::static_pointer_cast<FunctionCallStmt>(stmt);
        generateExpression(callStmt);
    }
    else if(stmt->type == StmtType::Return){
        auto ret = std::static_pointer_cast<ReturnStmt>(stmt);
        handleReturn(ret);
    }
    else if(stmt->type == StmtType::StructDecl){
        auto structStmt = std::static_pointer_cast<StructStmt>(stmt);
        handleStructDecl(structStmt);
    }    
}



llvm::Value* CodeGen::generateExpression(std::shared_ptr<Stmt> expr) {
    if(expr->type == StmtType::Literal) {
        auto literalStmt = std::static_pointer_cast<LiteralStmt>(expr);
        return generateLiteral(literalStmt);
    }
    else if(expr->type == StmtType::ArrayLiteral){ // alokasi  array, nanti ditunjuk oleh variabel
        auto arrStmt = std::static_pointer_cast<ArrayLiteralStmt>(expr);
        return generateArrayLiteral(arrStmt);
    }
    else if(expr->type == StmtType::Identifier) {
        auto identifierStmt = std::static_pointer_cast<IdentifierStmt>(expr);
        return generateIdentifier(identifierStmt);
    }
    else if(expr->type == StmtType::UnaryOp){
        auto unary = std::static_pointer_cast<UnaryOpStmt>(expr);
        return generateUnary(unary);
    }
    else if(expr->type == StmtType::BinaryOp) {
        auto binOp = std::static_pointer_cast<BinaryOpStmt>(expr);
        return generateBinary(binOp);
    }
    else if(expr->type == StmtType::Typeof) {
        auto typeofStmt = std::static_pointer_cast<TypeofStmt>(expr);
        return generateTypeof(typeofStmt);
    }
    else if(expr->type == StmtType::Input){
        auto inputStmt = std::static_pointer_cast<InputStmt>(expr);
        return generateInput(inputStmt);
    }
    else if(expr->type == StmtType::ArrayAccess){
        auto accessStmt = std::static_pointer_cast<ArrayAccessStmt>(expr);
        return generateArrayAccess(accessStmt);
    }
    else if(expr->type == StmtType::FunctionCall){
        auto callStmt = std::static_pointer_cast<FunctionCallStmt>(expr);
        return generateFunctionCall(callStmt);
    }
    if(expr->type == StmtType::StructInit){
        auto structInitStmt = std::static_pointer_cast<StructExpr>(expr);
        return generateStructInit(structInitStmt);
    }
    else if(expr->type == StmtType::MemberAccess){
        auto memberStmt = std::static_pointer_cast<MemberAccessExpr>(expr);
        return generateMemberAccess(memberStmt);
    }
    return nullptr;
}



void CodeGen::makeMainFunction(){
    llvm::FunctionType *mainFunType = llvm::FunctionType::get(builder.getInt32Ty(), false);
    mainFun = llvm::Function::Create(
        mainFunType,
        llvm::Function::ExternalLinkage,
        "main",
        module.get()
    );
    llvm::BasicBlock *mainFunEntry = llvm::BasicBlock::Create(context, "mainFunEntry", mainFun);
    builder.SetInsertPoint(mainFunEntry);

    // panggil yang lain disini
    generateProgram(this->ast_tree);

    // return 0
    builder.CreateRet(builder.getInt32(0));
}



llvm::AllocaInst* CodeGen::createEntryBlockAlloca(
    llvm::Function* func, const std::string& varName,
    llvm::Type *type, int n
) {
    llvm::IRBuilder<> tmpB(&func->getEntryBlock(), func->getEntryBlock().begin());
    llvm::Value* arraySize = nullptr;
    if (n > 1) arraySize = llvm::ConstantInt::get(tmpB.getInt32Ty(), n);

    return tmpB.CreateAlloca(type, arraySize, varName.c_str());
}



void CodeGen::enterScope() { VariablesStack.push_back({}); }
void CodeGen::exitScope() { VariablesStack.pop_back(); }



VarInfo* CodeGen::lookupVariable(const std::string& name) {
    for (auto it = VariablesStack.rbegin(); it != VariablesStack.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) {
            // std::cout << found->first << "\n";
            return &found->second;
        }
    }
    return nullptr; // tidak ditemukan
}



void CodeGen::run(bool printIr){
    if(printIr) module->print(llvm::outs(), nullptr);
    std::error_code EC;
    llvm::raw_fd_ostream file("./out/main.ll", EC);
    module->print(file, nullptr); // `module` adalah llvm::Module
}



size_t CodeGen::sizeofArray(const std::shared_ptr<ArrayLiteralStmt>& arrStmt){
    return arrStmt->elements.size();
}



llvm::Value* CodeGen::getBracketString(bool left) {
    static llvm::GlobalVariable* leftGlobal = nullptr;
    static llvm::GlobalVariable* rightGlobal = nullptr;

    if (!leftGlobal && !rightGlobal) {
        leftGlobal = builder.CreateGlobalString("[", "leftBracket");
        rightGlobal = builder.CreateGlobalString("]", "rightBracket");
    }

    return left ? leftGlobal : rightGlobal;
}





llvm::Type* CodeGen::literalTypeToLLVM(LiteralType type) {
    switch(type) {
        case LiteralType::INT_32: return builder.getInt32Ty();
        case LiteralType::INT_64: return builder.getInt64Ty();
        case LiteralType::DOUBLE: return builder.getDoubleTy();
        case LiteralType::BOOL: return builder.getInt1Ty();
        case LiteralType::STRING: return builder.getInt8PtrTy();
        default: return builder.getInt32Ty(); // fallback
    }
}

llvm::Constant* CodeGen::defaultValueForType(LiteralType type) {
    switch(type) {
        case LiteralType::INT_32: return llvm::ConstantInt::get(builder.getInt32Ty(), 0);
        case LiteralType::INT_64: return llvm::ConstantInt::get(builder.getInt64Ty(), 0);
        case LiteralType::DOUBLE: return llvm::ConstantFP::get(builder.getDoubleTy(), 0.0);
        case LiteralType::BOOL: return llvm::ConstantInt::get(builder.getInt1Ty(), 0);
        case LiteralType::STRING: return llvm::ConstantPointerNull::get(builder.getInt8PtrTy());
        default: return llvm::ConstantInt::get(builder.getInt32Ty(), 0);
    }
}

LiteralType CodeGen::inferLiteralTypeFromLLVM(llvm::Type* t) {
    if(t->isIntegerTy(32)) return LiteralType::INT_32;
    if(t->isIntegerTy(64)) return LiteralType::INT_64;
    if(t->isIntegerTy(1)) return LiteralType::BOOL;
    if(t->isDoubleTy()) return LiteralType::DOUBLE;
    if(t->isPointerTy() && t->getPointerElementType()->isIntegerTy(8)) return LiteralType::STRING;
    return LiteralType::UNKNOWN;
}
