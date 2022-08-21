//
// Created by seeu on 2022/7/9.
//

#include "Ast.h"
#include <sstream>
#include "fmt/core.h"
#include "fmt/ostream.h"

namespace Common {
    std::string Program::toString() {
        std::stringstream ss;
        for_each(statements.begin(), statements.end(), [&](auto &stmt) {
            ss << stmt->toString();
        });
        return ss.str();
    }

    std::string LetStatement::toString() {
        std::stringstream ss;
        ss << fmt::format("{} {}", token.literal, name->toString());
        if (value != nullptr) {
            ss << fmt::format(" = {}", value->toString());
        }
        ss << ";";
        return ss.str();
    }

    std::string Identifier::toString() {
        return value;
    }

    std::string PrefixExpression::toString() {
        std::stringstream ss;
        ss << fmt::format("({}{})", prefixOperator, rightExpression->toString());
        return ss.str();
    }

    std::string IfExpression::toString() {
        std::stringstream ss;
        ss << fmt::format("if {} {}", condition->toString(), consequence->toString());
        if (alternative != nullptr) {
            ss << "else " << alternative->toString();
        }
        return ss.str();
    }

    std::string FunctionExpression::toString() {
        vector<string> params{};
        transform(parameters.begin(), parameters.end(), params.begin(), [](auto &p) {
            return p->toString();
        });
        return fmt::format("fn({}){}", fmt::join(params, ", "), body->toString());
    }

    std::string BlockStatement::toString() {
        std::stringstream ss;
        for (auto &stmt: statements) {
            ss << stmt->toString();
        }
        return ss.str();
    }

    std::string ReturnStatement::toString() {
        return fmt::format("return {}", returnValue->toString());
    }

    std::string ExpressionStatement::toString() {
        return expression->toString();
    }

    std::string InfixExpression::toString() {
        return fmt::format("({} {} {})", leftExpression->toString(), infixOperator, rightExpression->toString());
    }

    std::string CallExpression::toString() {
        vector<string> args{};
        transform(arguments.begin(), arguments.end(), args.begin(), [](auto &p) {
            return p->toString();
        });
        return fmt::format("{}({})", name->toString(), fmt::join(args, ", "));
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
        vector<string> elems{};
        transform(elements.begin(), elements.end(), elems.begin(), [](auto &e) {
            return e->toString();
        });
        return fmt::format("[{}]", fmt::join(elems, ", "));
    }

}
