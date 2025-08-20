#ifndef AST_HPP
#define AST_HPP

#include <string>
#include <vector>
#include <memory>
#include <iostream>

#include <charconv> // from chars (int)
#include <cstdlib> // strtod
#include <cctype>
#include <optional>
#include <variant>
#include <iomanip>

enum class LiteralType {
    UNKNOWN, INT_32, INT_64, FLOAT, DOUBLE, STRING, BOOL, STRUCT
};

enum class StmtType {
    Program, VarDecl, Assignment,
    Print, If, Elif, For,
    ArrayLiteral, ArrayAccess, UnaryOp,
    FunctionDecl, Return, Break, Continue,
    BinaryOp, Literal, Identifier,
    FunctionCall, Typeof, Input, 
    StructDecl, StructInit, MemberAccess
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
    LiteralType varType;
    std::shared_ptr<Stmt> initializer;
    bool hasExplicit;
    VarDeclStmt(const std::string n, std::shared_ptr<Stmt> init, LiteralType ty, bool explicitTy)
        : Stmt(StmtType::VarDecl), name(n), initializer(init), varType(ty), hasExplicit(explicitTy) {}
};



// -------------------- Assignment --------------------
struct AssignmentStmt: public Stmt {
    std::string name;
    std::shared_ptr<Stmt> value;
    std::shared_ptr<Stmt> index;
    AssignmentStmt(const std::string& n, std::shared_ptr<Stmt> val, std::shared_ptr<Stmt> idx = nullptr)
        : Stmt(StmtType::Assignment), name(n), value(val), index(idx) {}
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
    std::vector<std::shared_ptr<Stmt>> params;
    std::vector<std::shared_ptr<Stmt>> body;

    FunctionDeclStmt(const std::string& n,
                     const std::vector<std::shared_ptr<Stmt>>& p,
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



struct UnaryOpStmt : public Stmt {
    std::string op;
    std::shared_ptr<Stmt> operand;
    UnaryOpStmt(const std::string& o, std::shared_ptr<Stmt> expr)
        : Stmt(StmtType::UnaryOp), op(o), operand(expr) {}
};



struct LiteralStmt : public Stmt {
    std::variant<int32_t, int64_t, double, std::string, bool> value;
    LiteralType dataType = LiteralType::UNKNOWN;

    explicit LiteralStmt(const std::string& s)
        : Stmt(StmtType::Literal), value(s), dataType(LiteralType::STRING) {}
    
    explicit LiteralStmt(int32_t i)
        : Stmt(StmtType::Literal), value(i), dataType(LiteralType::INT_32) {}
    
    explicit LiteralStmt(int64_t i)
        : Stmt(StmtType::Literal), value(i), dataType(LiteralType::INT_64) {}
    
    explicit LiteralStmt(float f)
        : Stmt(StmtType::Literal), value(static_cast<double>(f)), dataType(LiteralType::DOUBLE) {}
    
    explicit LiteralStmt(double d)
        : Stmt(StmtType::Literal), value(d), dataType(LiteralType::DOUBLE) {}
    
    explicit LiteralStmt(bool b)
        : Stmt(StmtType::Literal), value(b), dataType(LiteralType::BOOL) {}
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
    std::shared_ptr<Stmt> expr;
    std::string dataType;
    InputStmt(std::shared_ptr<Stmt> e, const std::string& ty = "") : Stmt(StmtType::Input), expr(e), dataType(ty) {}
};

        
struct BreakStmt : public Stmt {
    BreakStmt() : Stmt(StmtType::Break) {}
};

struct ContinueStmt : public Stmt {
    ContinueStmt() : Stmt(StmtType::Continue) {}
};        



// -------------------- Struct --------------------
struct StructField {
    std::string name;
    LiteralType type;

    StructField(std::string& n, LiteralType ty)
        : name(n), type(ty) {}
};

struct StructStmt : public Stmt {
    std::string name;
    std::vector<std::shared_ptr<StructField>> fields;
    
    StructStmt(const std::string& n, const std::vector<std::shared_ptr<StructField>>& fld)
        : Stmt(StmtType::StructDecl), name(n), fields(fld) {}
};


struct StructExpr : public Stmt {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<Stmt>>> fieldsValue;
    
    StructExpr( const std::string& n,
                std::vector<std::pair<std::string, std::shared_ptr<Stmt>>> fv)
        : Stmt(StmtType::StructInit), name(n), fieldsValue(fv) {}
};


struct MemberAccessExpr : Stmt {
    std::shared_ptr<Stmt> object;
    std::string memberName;

    MemberAccessExpr(std::shared_ptr<Stmt> obj, const std::string& mem)
        : Stmt(StmtType::MemberAccess), object(obj), memberName(mem) {}
};









#include <iostream>

inline void indent(int level) {
    for (int i = 0; i < level; ++i)
        std::cout << "  ";
}

inline void printStmt(const std::shared_ptr<Stmt>& stmt, int level = 0) {
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
            indent(level);
        
            // cek tipe dan print sesuai
            if (std::holds_alternative<int32_t>(lit->value)) {
                std::cout << "Literal (int32): " << std::get<int32_t>(lit->value) << std::endl;   
            }
            else if (std::holds_alternative<int64_t>(lit->value)) {
                std::cout << "Literal (int64): " << std::get<int64_t>(lit->value) << std::endl;
            }
            // else if (std::holds_alternative<float>(lit->value)) {
            //     std::ostringstream oss;
            //     oss << std::fixed << std::setprecision(6) << std::get<float>(lit->value);
            //     std::cout << "Literal (float): " << oss.str() << std::endl;
            // }
            else if (std::holds_alternative<double>(lit->value)) {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << std::get<double>(lit->value);
                std::cout << "Literal (double): " << oss.str() << std::endl;
            }
            else if (std::holds_alternative<std::string>(lit->value)) {
                std::cout << "Literal (string): \"" << std::get<std::string>(lit->value) << "\"" << std::endl;
            }
            else if (std::holds_alternative<bool>(lit->value)) {
                std::cout << "Literal (bool): " << (std::get<bool>(lit->value) ? "true" : "false") << std::endl;
            }
        
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
        
            indent(level + 1); std::cout << "Params: \n";
            for (const auto& p : fn->params)
            printStmt(p, level + 2);
        
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
            indent(level); std::cout << "Size:" << arr->elements.size() << std::endl;
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
            auto input = std::static_pointer_cast<InputStmt>(stmt);
            indent(level); std::cout << "Input" << std::endl;
            printStmt(input->expr, level + 1);
            break;
        }

        case StmtType::Typeof: {
            auto type = std::static_pointer_cast<TypeofStmt>(stmt);
            indent(level); std::cout << "Typeof" << std::endl;
            printStmt(type->expression, level + 1);
            break;
        }

        case StmtType::Return: {
            auto ret = std::static_pointer_cast<ReturnStmt>(stmt);
            indent(level); std::cout << "Return" << std::endl;
            printStmt(ret->value, level + 1);
            break;
        }


        case StmtType::Assignment: {
            auto assign = std::static_pointer_cast<AssignmentStmt>(stmt);
            indent(level); std::cout << "Assignment: " << assign->name << std::endl;
            printStmt(assign->value, level + 1);
            if(assign->index) printStmt(assign->value, level + 1);
            break;
        }
        
        case StmtType::UnaryOp: {
            auto unary = std::static_pointer_cast<UnaryOpStmt>(stmt);
            indent(level); std::cout << "UnaryOp: " << unary->op << std::endl;
            printStmt(unary->operand, level + 1);
            break;
        }


        case StmtType::Break: {
            indent(level); std::cout << "Break" << std::endl;
            break;
        }
        
        case StmtType::Continue: {
            indent(level); std::cout << "Continue" << std::endl;
            break;
        }



        case StmtType::StructDecl: {
            auto structStmt = std::static_pointer_cast<StructStmt>(stmt);
            indent(level); 
            std::cout << "StructDecl: " << structStmt->name << std::endl;
        
            for (const auto& field : structStmt->fields) {
                indent(level + 1);
                std::cout << "Field: " << field->name 
                          << " (type: " << static_cast<int>(field->type) << ")" << std::endl;
            }            
            break;
        }


        case StmtType::StructInit: {
            auto structExpr = std::static_pointer_cast<StructExpr>(stmt);
            indent(level);
            std::cout << "StructExpr: " << structExpr->name << std::endl;
        
            for (const auto& fieldPair : structExpr->fieldsValue) {
                indent(level + 1);
                std::cout << "Field: " << fieldPair.first << " = ";
        
                // Kita print value field berdasarkan tipenya
                auto valueStmt = fieldPair.second;
                if (!valueStmt) {
                    std::cout << "null" << std::endl;
                    continue;
                }
        
                switch (valueStmt->type) {
                    case StmtType::Literal: {
                        auto lit = std::static_pointer_cast<LiteralStmt>(valueStmt);
                        printStmt(lit);
                        break;
                    }
                    case StmtType::BinaryOp: {
                        std::cout << "<BinaryOp>" << std::endl;
                        break;
                    }
                    case StmtType::Identifier: {
                        auto id = std::static_pointer_cast<IdentifierStmt>(valueStmt);
                        std::cout << id->name << std::endl;
                        break;
                    }
                    default:
                        std::cout << "<Expr>" << std::endl;
                }
            }
            break;
        }        
        

        case StmtType::MemberAccess: {
            auto member = std::static_pointer_cast<MemberAccessExpr>(stmt);
            indent(level);
            std::cout << "MemberAccessExpr:" << std::endl;

            indent(level + 1);
            std::cout << "object:" << std::endl;
            printStmt(member->object, level + 2);

            indent(level + 2);
            std::cout << "memberName: " << member->memberName << std::endl;
            break;
        }
        


        default: {
            indent(level); std::cout << "Unknown StmtType" << std::endl;
            break;
        }
    }
}





#endif