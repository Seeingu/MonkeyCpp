//
// Created by seeu on 2022/7/9.
//

#include "Ast.h"
#include <sstream>

namespace GI {
    std::string Program::toString() {
        std::stringstream ss;
        for (auto &stmt: statements) {
            ss << stmt->toString();
        }
        return ss.str();
    }

    std::string LetStatement::toString() {
        std::stringstream ss;
        ss << token.literal << " ";
        ss << name->toString();
        ss << " = ";
        if (value != nullptr) {
            ss << value->toString();
        }
        ss << ";";
        return ss.str();
    }

    std::string Identifier::toString() {
        return value;
    }

    std::string PrefixExpression::toString() {
        std::stringstream ss;
        ss << "(" << prefixOperator;
        ss << rightExpression->toString() << ")";
        return ss.str();
    }

    std::string IfExpression::toString() {
        std::stringstream ss;
        ss << "if ";
        ss << condition->toString();
        ss << " " << consequence->toString();
        if (alternative != nullptr) {
            ss << "else " << alternative->toString();
        }
        return ss.str();
    }

    std::string FunctionExpression::toString() {
        std::stringstream ss;
        std::stringstream paramsStream;
        for (auto &param: parameters) {
            paramsStream << param->toString() << ", ";
        }
        // remove end comma
        paramsStream.seekp(-2, std::ios_base::end);
        ss << "fn(";
        ss << paramsStream.str();
        ss << ")";
        ss << body->toString();
        return ss.str();
    }

    std::string BlockStatement::toString() {
        std::stringstream ss;
        for (auto &stmt: statements) {
            ss << stmt->toString();
        }
        return ss.str();
    }

    std::string ReturnStatement::toString() {
        std::stringstream ss;
        ss << "return " << returnValue->toString();
        return ss.str();
    }

    std::string ExpressionStatement::toString() {
        return expression->toString();
    }

    std::string InfixExpression::toString() {
        std::stringstream ss;
        ss << "(" << leftExpression->toString();
        ss << " " << infixOperator << " ";
        ss << rightExpression->toString() << ")";
        return ss.str();
    }

    std::string CallExpression::toString() {
        std::stringstream ss;
        ss << name->toString() << "(";
        std::stringstream argsStream;
        for (auto &arg: arguments) {
            argsStream << arg->toString() << ", ";
        }
        argsStream.seekp(-2, std::ios::end);
        ss << argsStream.str() << ")";
        return ss.str();
    }

    string HashExpression::toString() {
        stringstream ss{};
        ss << "{";
        for (auto &pair: pairs) {
            ss << pair.first->toString() << ": " << pair.second->toString() << "," << std::endl;
        }
        ss << "}";
        return ss.str();
    }

    std::string ArrayExpression::toString() {
        std::ostringstream ss;
        ss << "[";
        std::ostringstream elemStream;
        for (auto &elem: elements) {
            elemStream << elem->toString() << ", ";
        }
        elemStream.seekp(-2, std::ios::end);
        ss << elemStream.str();
        ss << "]";
        return ss.str();
    }

}
