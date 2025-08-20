#include "../../include/CodeGen.hpp"

llvm::Value* CodeGen::generateLiteral(const std::shared_ptr<LiteralStmt>& literalStmt){
    switch(literalStmt->dataType) {
        case LiteralType::INT_32: {
            int32_t val = std::get<int32_t>(literalStmt->value);
            return llvm::ConstantInt::get(builder.getInt32Ty(), val, true);
        }
        case LiteralType::INT_64: {
            int64_t val = std::get<int64_t>(literalStmt->value);
            return llvm::ConstantInt::get(builder.getInt64Ty(), val, true);
        }
        // case LiteralType::FLOAT: {
    
        //     double val = std::get<double>(literalStmt->value);
        //     return llvm::ConstantFP::get(builder.getFloat(), val);
        //     if (auto ptr = std::get_if<double>(&literalStmt->value)) {
        //         double val = *ptr;
        //         return llvm::ConstantFP::get(builder.getDoubleTy(), val);
        //     } else {
        //         throw std::runtime_error("LiteralType DOUBLE tapi variant bukan double");
        //     }
        // }
        case LiteralType::DOUBLE: {
            double val = std::get<double>(literalStmt->value);
            return llvm::ConstantFP::get(builder.getDoubleTy(), val);
            // if (auto ptr = std::get_if<double>(&literalStmt->value)) {
            //     double val = *ptr;
            //     return llvm::ConstantFP::get(builder.getDoubleTy(), val);
            // } else {
            //     throw std::runtime_error("LiteralType DOUBLE tapi variant bukan double");
            // }
        }
        case LiteralType::BOOL: {
            bool val = std::get<bool>(literalStmt->value);
            return llvm::ConstantInt::get(builder.getInt1Ty(), val);
        }
        case LiteralType::STRING: {
            std::string val = std::get<std::string>(literalStmt->value);
            return builder.CreateGlobalStringPtr(val);
        }
        case LiteralType::UNKNOWN: {
            throw std::runtime_error("Data type  is unknown\n");
        }
        default: {
            throw std::runtime_error("Data type is null");
        }
    }
}
