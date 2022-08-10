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
                return;
            }
            case GI::NodeType::InfixExpression: {
                auto expr = static_cast<GI::InfixExpression *>(node);
                compile(expr->leftExpression.get());
                compile(expr->rightExpression.get());

                if (expr->infixOperator == "+") {
                    emit(OpCode::Add);
                } else {
                    throw "unsupported operator: " + expr->infixOperator;
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

        }

    }

    int Compiler::emit(OpCode opCode, vector<int> operands) {
        auto ins = code.makeInstruction(opCode, std::move(operands));
        return addInstruction(ins);
    }

    int Compiler::addConstant(shared_ptr<GI::GIObject> object) {
        constants.push_back(std::move(object));
        return constants.size() - 1;
    }

    int Compiler::addInstruction(Instruction instruction) {
        auto pos = instructions.size();
        instructions.insert(instructions.end(), instruction.begin(), instruction.end());
        return pos;
    }
}

#pragma clang diagnostic pop
