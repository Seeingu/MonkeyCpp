#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//
// Created by seeu on 2022/8/9.
//

#include "Compiler.h"

#include <utility>

namespace GC {
    void Compiler::compile(GI::Node *node) {
        switch (node->getType()) {
            case GI::NodeType::Program: {
                auto program = static_cast<GI::Program *>(node);

                for (auto &stmt: program->statements) {
                    compile(stmt.get());
                }
                return;
            }
            case GI::NodeType::ExpressionStatement: {
                compile(static_cast<GI::ExpressionStatement *>(node)->expression.get());
                emit(OpCode::Pop);
                return;
            }
            case GI::NodeType::InfixExpression: {
                auto expr = static_cast<GI::InfixExpression *>(node);
                if (expr->infixOperator == "<") {
                    compile(expr->rightExpression.get());
                    compile(expr->leftExpression.get());
                    emit(OpCode::GreaterThan);
                    return;
                }

                compile(expr->leftExpression.get());
                compile(expr->rightExpression.get());


                std::map<string, OpCode> infixActions{
                        {"+",  OpCode::Add},
                        {"-",  OpCode::Sub},
                        {"*",  OpCode::Mul},
                        {"/",  OpCode::Div},
                        {"==", OpCode::Equal},
                        {"!=", OpCode::NotEqual},
                        {">",  OpCode::GreaterThan},
                };
                if (!infixActions.contains(expr->infixOperator)) {
                    throw "unsupported operator: " + expr->infixOperator;
                }
                emit(infixActions[expr->infixOperator]);
                return;
            }
            case GI::NodeType::PrefixExpression: {
                auto prefixExpr = static_cast<GI::PrefixExpression *>(node);
                compile(prefixExpr->rightExpression.get());
                if (prefixExpr->prefixOperator == "!") {
                    emit(OpCode::Bang);
                } else if (prefixExpr->prefixOperator == "-") {
                    emit(OpCode::Minus);
                } else {
                    throw "unsupported operator: " + prefixExpr->prefixOperator;
                }
                return;
            }
            case GI::NodeType::IntegerExpression: {
                auto integerExpr = static_cast<GI::IntegerExpression *>(node);
                emit(OpCode::Constant, {
                        addConstant(make_unique<GI::IntegerObject>(integerExpr->value))
                });
                return;
            }
            case GI::NodeType::BoolExpression: {
                auto boolExpr = static_cast<GI::BoolExpression *>(node);
                emit(boolExpr->value ? OpCode::True : OpCode::False);
                return;
            }

            default:
                break;
        }

    }

    int Compiler::emit(OpCode opCode, vector<int> operands) {
        auto ins = code.makeInstruction(opCode, std::move(operands));
        return addInstruction(ins);
    }

    int Compiler::addConstant(shared_ptr<GI::GIObject> object) {
        constants.push_back(std::move(object));
        return int(constants.size()) - 1;
    }

    int Compiler::addInstruction(Instruction instruction) {
        auto pos = instructions.size();
        instructions.insert(instructions.end(), instruction.begin(), instruction.end());
        return int(pos);
    }
}

#pragma clang diagnostic pop
