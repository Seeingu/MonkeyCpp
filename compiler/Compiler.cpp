#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//
// Created by seeu on 2022/8/9.
//

#include "Compiler.h"
#include "fmt/core.h"
#include "magic_enum.hpp"

#include <algorithm>
#include <utility>

#define DUMB_INSTRUCTION_ADDRESS 9999

namespace GC {
    void Compiler::compile(Common::Node *node) {
        switch (node->getType()) {
            case Common::NodeType::Program: {
                auto program = static_cast<Common::Program *>(node);

                for (auto &stmt: program->statements) {
                    compile(stmt.get());
                }
                break;
            }
            case Common::NodeType::ExpressionStatement: {
                compile(static_cast<Common::ExpressionStatement *>(node)->expression.get());
                emit(OpCode::Pop);
                break;
            }
            case Common::NodeType::InfixExpression: {
                auto expr = static_cast<Common::InfixExpression *>(node);
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
                break;
            }
            case Common::NodeType::PrefixExpression: {
                auto prefixExpr = static_cast<Common::PrefixExpression *>(node);
                compile(prefixExpr->rightExpression.get());
                if (prefixExpr->prefixOperator == "!") {
                    emit(OpCode::Bang);
                } else if (prefixExpr->prefixOperator == "-") {
                    emit(OpCode::Minus);
                } else {
                    throw "unsupported operator: " + prefixExpr->prefixOperator;
                }
                break;
            }
            case Common::NodeType::IntegerExpression: {
                auto integerExpr = static_cast<Common::IntegerExpression *>(node);
                emit(OpCode::Constant, {
                        addConstant(make_unique<Common::IntegerObject>(integerExpr->value))
                });
                break;
            }
            case Common::NodeType::BoolExpression: {
                auto boolExpr = static_cast<Common::BoolExpression *>(node);
                emit(boolExpr->value ? OpCode::True : OpCode::False);
                break;
            }
            case Common::NodeType::IfExpression: {
                auto ifExpr = static_cast<Common::IfExpression *>(node);
                compile(ifExpr->condition.get());

                auto jumpNotTruthyPos = emit(OpCode::JumpNotTruthy, {DUMB_INSTRUCTION_ADDRESS});

                compile(ifExpr->consequence.get());

                auto jumpPos = emit(OpCode::Jump, {DUMB_INSTRUCTION_ADDRESS});
                int afterConsequencePos = instructions.size();
                changeOperand(jumpNotTruthyPos, {afterConsequencePos});

                if (ifExpr->alternative == nullptr) {
                    emit(OpCode::_Null);
                } else {
                    compile(ifExpr->alternative.get());
                }

                int afterAlternativePos = instructions.size();
                changeOperand(jumpPos, {afterAlternativePos});
                break;
            }
            case Common::NodeType::BlockStatement: {
                auto blockStmt = static_cast<Common::BlockStatement *>(node);
                for (auto &stmt: blockStmt->statements) {
                    compile(stmt.get());
                }
                instructions.pop_back();

                break;
            }
            case Common::NodeType::LetStatement: {
                auto letStmt = static_cast<Common::LetStatement *>(node);
                compile(letStmt->value.get());

                auto symbol = symbolTable.define(letStmt->name->value);
                emit(OpCode::SetGlobal, {symbol.index});
                break;
            }
            case Common::NodeType::Identifier: {
                auto id = static_cast<Common::Identifier *>(node);
                auto symbol = symbolTable.resolve(id->value);
                if (symbol.has_value()) {
                    emit(OpCode::GetGlobal, {symbol.value().index});
                } else {
                    throw fmt::format("undefined variable {}", id->value);
                }
                break;
            }
            case Common::NodeType::StringExpression: {
                auto stringExpr = static_cast<Common::StringExpression *>(node);
                emit(OpCode::Constant, {
                        addConstant(make_unique<Common::StringObject>(stringExpr->value))
                });
                break;
            }
            case Common::NodeType::ArrayExpression: {
                auto arrayExpr = static_cast<Common::ArrayExpression *>(node);
                for (auto &item: arrayExpr->elements) {
                    compile(item.get());
                }

                emit(OpCode::Array, {int(arrayExpr->elements.size())});
                break;
            }
            case Common::NodeType::HashExpression: {
                auto hashExpr = static_cast<Common::HashExpression *>(node);
                int size = hashExpr->pairs.size() * 2;
                using HashPair = pair<Common::Expression *, Common::Expression *>;
                vector<HashPair> pairs{};
                for (auto &p: hashExpr->pairs) {
                    pairs.emplace_back(p.first.get(), p.second.get());
                }
                std::sort(pairs.begin(), pairs.end(), [](const HashPair &p1, const HashPair &p2) {
                    return p1.first->toString() < p2.first->toString();
                });
                for (auto &p: pairs) {
                    compile(p.first);
                    compile(p.second);
                }

                emit(OpCode::Hash, {size});
                break;
            }
            case Common::NodeType::IndexExpression: {
                auto indexExpr = static_cast<Common::IndexExpression *>(node);
                compile(indexExpr->leftExpression.get());
                compile(indexExpr->indexExpression.get());

                emit(OpCode::Index);
                break;
            }
            default:
                throw fmt::format("unsupported node type: {}", magic_enum::enum_name(node->getType()));
        }

    }

    int Compiler::emit(OpCode opCode, vector<int> operands) {
        auto ins = code.makeInstruction(opCode, std::move(operands));
        auto pos = addInstruction(ins);

        // setLastInstruction(opCode, pos);

        return pos;
    }


    void Compiler::setLastInstruction(OpCode code, int position) {
        previousInstruction = lastInstruction;
        lastInstruction = EmittedInstruction{code, position};
    }

    int Compiler::addConstant(shared_ptr<Common::GIObject> object) {
        constants.push_back(std::move(object));
        return int(constants.size()) - 1;
    }

    int Compiler::addInstruction(Instruction instruction) {
        auto pos = instructions.size();
        instructions.insert(instructions.end(), instruction.begin(), instruction.end());
        return int(pos);
    }

    void Compiler::changeOperand(int position, vector<int> operand) {
        auto opCode = OpCode(instructions[position]);
        auto ins = code.makeInstruction(opCode, operand);

        for (int i = 0; i < ins.size(); i++) {
            instructions[position + i] = ins[i];
        }

    }
}

#pragma clang diagnostic pop
