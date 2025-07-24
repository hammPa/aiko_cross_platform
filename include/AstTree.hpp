#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

enum class StmtType {
    Program, VarDecl, Print,
    If, Elif, For,
    ArrayLiteral, ArrayAccess,
    FunctionDecl, Return,
    BinaryOp, Literal, Identifier,
    FunctionCall, Typeof, Input
};

struct Stmt {
    StmtType type;
    Stmt(StmtType t) : type(t) {}
    virtual ~Stmt() = default;
};

// masa gapaham sin ? ini pakai inheritance agar semua turunan bisa di simpan dalam 1 vector induk
struct ProgramStmt: public Stmt {
    std::vector<std::shared_ptr<Stmt>> statements;
    ProgramStmt(const std::vector<std::shared_ptr<Stmt>>& stmts)
        : Stmt(StmtType::Program), statements(stmts) {}
};


// -------------------- VarDecl --------------------
struct VarDeclStmt: public Stmt {
    std::string name;
    std::shared_ptr<Stmt> initializer;
    VarDeclStmt(const std::string n, std::shared_ptr<Stmt> init)
        : Stmt(StmtType::VarDecl), name(n), initializer(init) {}
};



// -------------------- Print --------------------
struct PrintStmt: public Stmt {
    std::shared_ptr<Stmt> expression;
    PrintStmt(std::shared_ptr<Stmt> expr)
        : Stmt(StmtType::Print), expression(expr) {}
};



// -------------------- If / Elif / Else --------------------
struct ElifStmt: public Stmt {
    std::shared_ptr<Stmt> condition;
    std::vector<std::shared_ptr<Stmt>> block;
    ElifStmt(std::shared_ptr<Stmt> cond, const std::vector<std::shared_ptr<Stmt>>& blk)
        : Stmt(StmtType::Elif), condition(cond), block(blk) {}
};


struct IfStmt: public Stmt {
    std::shared_ptr<Stmt> condition;
    std::vector<std::shared_ptr<Stmt>> then_block;
    std::vector<std::shared_ptr<ElifStmt>> elifs;
    std::vector<std::shared_ptr<Stmt>> else_block;

    IfStmt( std::shared_ptr<Stmt> cond,
            const std::vector<std::shared_ptr<Stmt>>& then_blk,
            const std::vector<std::shared_ptr<ElifStmt>>& el,
            const std::vector<std::shared_ptr<Stmt>>& else_blk)
        :  Stmt(StmtType::If),
            condition(cond),
            then_block(then_blk),
            elifs(el),
            else_block(else_blk) {}
};


// -------------------- For --------------------
struct ForStmt: public Stmt {
    std::string var_name;
    std::shared_ptr<Stmt> start;
    std::shared_ptr<Stmt> end;
    std::shared_ptr<Stmt> step;
    std::vector<std::shared_ptr<Stmt>> block;

    ForStmt(const std::string& var,
        std::shared_ptr<Stmt> st,
        std::shared_ptr<Stmt> en,
        std::shared_ptr<Stmt> sp,
        const std::vector<std::shared_ptr<Stmt>>& blk)
    : Stmt(StmtType::For),
    var_name(var),
    start(st), end(en), step(sp), block(blk) {}
};


// -------------------- Array --------------------
struct ArrayLiteralStmt : public Stmt {
    std::vector<std::shared_ptr<Stmt>> elements;
    ArrayLiteralStmt(const std::vector<std::shared_ptr<Stmt>>& elems)
        : Stmt(StmtType::ArrayLiteral), elements(elems) {}
};

struct ArrayAccessStmt : public Stmt {
    std::string array_name;
    std::shared_ptr<Stmt> index;
    ArrayAccessStmt(const std::string& name, std::shared_ptr<Stmt> idx)
        : Stmt(StmtType::ArrayAccess), array_name(name), index(idx) {}
};

// -------------------- Function --------------------
struct FunctionDeclStmt : public Stmt {
    std::string name;
    std::vector<std::string> params;
    std::vector<std::shared_ptr<Stmt>> body;

    FunctionDeclStmt(const std::string& n,
                     const std::vector<std::string>& p,
                     const std::vector<std::shared_ptr<Stmt>>& b)
        : Stmt(StmtType::FunctionDecl), name(n), params(p), body(b) {}
};

struct ReturnStmt : public Stmt {
    std::shared_ptr<Stmt> value;
    ReturnStmt(std::shared_ptr<Stmt> val)
        : Stmt(StmtType::Return), value(val) {}
};

// -------------------- Expressions --------------------
struct BinaryOpStmt : public Stmt {
    std::shared_ptr<Stmt> left;
    std::string op;
    std::shared_ptr<Stmt> right;

    BinaryOpStmt(std::shared_ptr<Stmt> l, const std::string& o, std::shared_ptr<Stmt> r)
        : Stmt(StmtType::BinaryOp), left(l), op(o), right(r) {}
};

struct LiteralStmt : public Stmt {
    std::string value;
    LiteralStmt(const std::string& val)
        : Stmt(StmtType::Literal), value(val) {}
};

struct IdentifierStmt : public Stmt {
    std::string name;
    IdentifierStmt(const std::string& n)
        : Stmt(StmtType::Identifier), name(n) {}
};

struct FunctionCallStmt : public Stmt {
    std::string name;
    std::vector<std::shared_ptr<Stmt>> args;

    FunctionCallStmt(const std::string& n, const std::vector<std::shared_ptr<Stmt>>& a)
        : Stmt(StmtType::FunctionCall), name(n), args(a) {}
};

struct TypeofStmt : public Stmt {
    std::shared_ptr<Stmt> expression;
    TypeofStmt(std::shared_ptr<Stmt> expr)
        : Stmt(StmtType::Typeof), expression(expr) {}
};

struct InputStmt : public Stmt {
    InputStmt() : Stmt(StmtType::Input) {}
};















#include <iostream>

void indent(int level) {
    for (int i = 0; i < level; ++i)
        std::cout << "  ";
}

void printStmt(const std::shared_ptr<Stmt>& stmt, int level = 0) {
    if (!stmt) {
        indent(level); std::cout << "(null stmt)" << std::endl;
        return;
    }

    switch (stmt->type) {
        case StmtType::Program: {
            auto prog = std::static_pointer_cast<ProgramStmt>(stmt);
            indent(level); std::cout << "Program:" << std::endl;
            for (const auto& s : prog->statements)
                printStmt(s, level + 1);
            break;
        }

        case StmtType::VarDecl: {
            auto var = std::static_pointer_cast<VarDeclStmt>(stmt);
            indent(level); std::cout << "VarDecl: " << var->name << std::endl;
            printStmt(var->initializer, level + 1);
            break;
        }

        case StmtType::Print: {
            auto pr = std::static_pointer_cast<PrintStmt>(stmt);
            indent(level); std::cout << "Print" << std::endl;
            printStmt(pr->expression, level + 1);
            break;
        }

        case StmtType::Literal: {
            auto lit = std::static_pointer_cast<LiteralStmt>(stmt);
            indent(level); std::cout << "Literal: " << lit->value << std::endl;
            break;
        }

        case StmtType::Identifier: {
            auto id = std::static_pointer_cast<IdentifierStmt>(stmt);
            indent(level); std::cout << "Identifier: " << id->name << std::endl;
            break;
        }

        case StmtType::BinaryOp: {
            auto bin = std::static_pointer_cast<BinaryOpStmt>(stmt);
            indent(level); std::cout << "BinaryOp: " << bin->op << std::endl;
            printStmt(bin->left, level + 1);
            printStmt(bin->right, level + 1);
            break;
        }

        case StmtType::If: {
            auto ifs = std::static_pointer_cast<IfStmt>(stmt);
            indent(level); std::cout << "If Condition:" << std::endl;
            printStmt(ifs->condition, level + 1);

            indent(level); std::cout << "Then Block:" << std::endl;
            for (const auto& s : ifs->then_block)
                printStmt(s, level + 1);

            for (const auto& elif : ifs->elifs) {
                indent(level); std::cout << "Elif Condition:" << std::endl;
                printStmt(elif->condition, level + 1);
                indent(level); std::cout << "Elif Block:" << std::endl;
                for (const auto& s : elif->block)
                    printStmt(s, level + 1);
            }

            if (!ifs->else_block.empty()) {
                indent(level); std::cout << "Else Block:" << std::endl;
                for (const auto& s : ifs->else_block)
                    printStmt(s, level + 1);
            }
            break;
        }

        case StmtType::For: {
            auto fs = std::static_pointer_cast<ForStmt>(stmt);
            indent(level); std::cout << "For " << fs->var_name << std::endl;
        
            indent(level + 1); std::cout << "Start:" << std::endl;
            printStmt(fs->start, level + 2);
        
            indent(level + 1); std::cout << "End:" << std::endl;
            printStmt(fs->end, level + 2);
        
            indent(level + 1); std::cout << "Step:" << std::endl;
            printStmt(fs->step, level + 2);
        
            indent(level + 1); std::cout << "Body:" << std::endl;
            for (const auto& s : fs->block)
                printStmt(s, level + 2);
            break;
        }


        case StmtType::FunctionDecl: {
            auto fn = std::static_pointer_cast<FunctionDeclStmt>(stmt);
            indent(level); std::cout << "Function: " << fn->name << std::endl;
        
            indent(level + 1); std::cout << "Params: ";
            for (const auto& p : fn->params)
                std::cout << p << " ";
            std::cout << std::endl;
        
            indent(level + 1); std::cout << "Body:" << std::endl;
            for (const auto& s : fn->body)
                printStmt(s, level + 2);
            break;
        }
        
        

        case StmtType::FunctionCall: {
            auto fc = std::static_pointer_cast<FunctionCallStmt>(stmt);
            indent(level); std::cout << "FunctionCall: " << fc->name << std::endl;
            for (const auto& arg : fc->args)
                printStmt(arg, level + 1);
            break;
        }

        case StmtType::ArrayLiteral: {
            auto arr = std::static_pointer_cast<ArrayLiteralStmt>(stmt);
            indent(level); std::cout << "ArrayLiteral:" << std::endl;
            for (const auto& e : arr->elements)
                printStmt(e, level + 1);
            break;
        }

        case StmtType::ArrayAccess: {
            auto acc = std::static_pointer_cast<ArrayAccessStmt>(stmt);
            indent(level); std::cout << "ArrayAccess: " << acc->array_name << std::endl;
            printStmt(acc->index, level + 1);
            break;
        }

        case StmtType::Input: {
            indent(level); std::cout << "Input" << std::endl;
            break;
        }

        case StmtType::Return: {
            auto ret = std::static_pointer_cast<ReturnStmt>(stmt);
            indent(level); std::cout << "Return" << std::endl;
            printStmt(ret->value, level + 1);
            break;
        }

        default: {
            indent(level); std::cout << "Unknown StmtType" << std::endl;
            break;
        }
    }
}





#endif