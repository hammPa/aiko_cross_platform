#ifndef CODEGEN_HPP
#define CODEGEN_HPP

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/FileSystem.h>

#include "AstTree.hpp"

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <stdexcept>
#include <utility>

typedef struct {
    llvm::AllocaInst* alloc;
    LiteralType dataType;
    bool isArray = false;
    size_t size = 1;
    bool staticType = false;
    std::string structTypeName = "";
} VarInfo;

struct StructInfo {
    llvm::StructType* type;
    std::unordered_map<std::string, unsigned> fieldIndices;
};

class CodeGen {
private:
    llvm::LLVMContext context;
    llvm::IRBuilder<> builder;
    std::unique_ptr<llvm::Module> module;
    std::vector<std::map<std::string, VarInfo>> VariablesStack;
    std::vector<llvm::BasicBlock*> BreakTargets;
    std::vector<llvm::BasicBlock*> ContinueTargets;
    std::shared_ptr<ProgramStmt> ast_tree;
    std::unordered_map<std::string, StructInfo> StructTypes;



    llvm::Function *printFun = nullptr;
    llvm::Function *mainFun = nullptr;
    bool mainFound = false;


    void handlePrint(const std::shared_ptr<PrintStmt>& printStmtObj, bool enter = true);
    void handlePrintArray(const VarInfo* varInfo);
    void handleVarDeclaration(const std::shared_ptr<VarDeclStmt>& varDeclStmt);
    void handleAssignment(const std::shared_ptr<AssignmentStmt>& assignmentStmt);
    void handleIf(const std::shared_ptr<IfStmt>& ifStmt);
    void handleFor(const std::shared_ptr<ForStmt>& forStmt);
    void handleReturn(const std::shared_ptr<ReturnStmt>& ret);
    void handleFunctionDecl(const std::shared_ptr<FunctionDeclStmt>& funcDecl);
    void handleStructDecl(const std::shared_ptr<StructStmt>& structStmt);

    llvm::Value* generateLiteral(const std::shared_ptr<LiteralStmt>& literalStmt);
    llvm::Value* generateArrayLiteral(const std::shared_ptr<ArrayLiteralStmt>& arrStmt);
    llvm::Value* generateIdentifier(const std::shared_ptr<IdentifierStmt>& identifierStmt);
    llvm::Value* generateUnary(const std::shared_ptr<UnaryOpStmt>& unary);
    llvm::Value* generateBinary(const std::shared_ptr<BinaryOpStmt>& binOp);
    llvm::Value* generateTypeof(const std::shared_ptr<TypeofStmt>& typeOfStmtObj);
    llvm::Value* generateInput(const std::shared_ptr<InputStmt>& inputStmt);
    llvm::Value* generateArrayAccess(const std::shared_ptr<ArrayAccessStmt>& accessStmt);
    llvm::Value* generateArrayElementPtr(VarInfo* var, llvm::Value* idxVal);
    llvm::Value* generateFunctionCall(const std::shared_ptr<FunctionCallStmt>& callStmt);
    llvm::Value* generateStructInit(const std::shared_ptr<StructExpr>& structInitStmt);
    llvm::Value* generateMemberAccess(const std::shared_ptr<MemberAccessExpr>& memberStmt);
    
    llvm::Function* getPrintf(llvm::Module *m);
    void genPrintString(llvm::Value *strVal, bool enter = true);
    void genPrintInt(llvm::Value *intVal, bool enter = true);
    void genPrintDouble(llvm::Value *floatVal, bool enter = true);



    void generateProgram(const std::shared_ptr<ProgramStmt>& ast_tree);
    void generateStatement(const std::shared_ptr<Stmt>& stmt);
    llvm::Value* generateExpression(std::shared_ptr<Stmt> expr);
    void makeMainFunction();
    llvm::AllocaInst* createEntryBlockAlloca(
        llvm::Function* func,
        const std::string& varName,
        llvm::Type *type,
        int n = 1
    );

    void enterScope();
    void exitScope();
    VarInfo* lookupVariable(const std::string& name);

    
    size_t sizeofArray(const std::shared_ptr<ArrayLiteralStmt>& arrStmt);
    llvm::Value* getBracketString(bool left);    
    llvm::Value* generateStringEq(llvm::Value* lhs, llvm::Value* rhs);


    llvm::Type* literalTypeToLLVM(LiteralType type);
    llvm::Constant* defaultValueForType(LiteralType type);
    LiteralType inferLiteralTypeFromLLVM(llvm::Type* t);
    

public:
    CodeGen(const std::shared_ptr<ProgramStmt>& ast_tree);
    ~CodeGen(){}
    void run(bool printIr = false);
};


#endif