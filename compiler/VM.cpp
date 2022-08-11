#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//
// Created by seeu on 2022/8/9.
//

#include "VM.h"
#include <string>

namespace GC {

    shared_ptr<GI::GIObject> VM::stackTop() {
        if (sp == 0) {
            return nullptr;
        }
        return stack[sp - 1];
    }

    shared_ptr<GI::GIObject> VM::lastStackElem() {
        return stack[sp];
    }

    void VM::run() {
        for (int i = 0; i < instructions.size(); i++) {
            auto instruction = instructions[i];
            auto opCode = OpCode(instruction);
            switch (opCode) {
                case OpCode::Constant: {
                    Instruction ins;
                    ins.assign(instructions.begin() + i + 1, instructions.end());
                    int constIndex = code.readUint16(ins);
                    i += 2;

                    stackPush(constants[constIndex]);
                    break;
                }
                case OpCode::Sub:
                case OpCode::Mul:
                case OpCode::Div:
                case OpCode::Add: {
                    auto right = stackPop();
                    auto left = stackPop();
                    auto leftValue = static_cast<GI::IntegerObject *>(left.get())->value;
                    auto rightValue = static_cast<GI::IntegerObject *>(right.get())->value;

                    int result;
                    switch (opCode) {
                        case OpCode::Add:
                            result = leftValue + rightValue;
                            break;
                        case OpCode::Sub:
                            result = leftValue - rightValue;
                            break;
                        case OpCode::Mul:
                            result = leftValue * rightValue;
                            break;
                        case OpCode::Div:
                            result = leftValue / rightValue;
                            break;
                        default:
                            // unreachable
                            break;
                    }
                    stackPush(make_shared<GI::IntegerObject>(result));
                    break;
                }
                case OpCode::True:
                case OpCode::False:
                    stackPush(make_shared<GI::BooleanObject>(opCode == OpCode::True));
                    break;
                case OpCode::Equal:
                case OpCode::NotEqual:
                case OpCode::GreaterThan: {
                    auto right = stackPop();
                    auto left = stackPop();
                    if (left->getType() == GI::ObjectType::INTEGER && right->getType() == GI::ObjectType::INTEGER) {
                        auto leftValue = static_cast<GI::IntegerObject *>(left.get())->value;
                        auto rightValue = static_cast<GI::IntegerObject *>(right.get())->value;
                        switch (opCode) {
                            case OpCode::Equal:
                                stackPush(make_shared<GI::BooleanObject>(leftValue == rightValue));
                                break;
                            case OpCode::NotEqual:
                                stackPush(make_shared<GI::BooleanObject>(leftValue != rightValue));
                                break;
                            case OpCode::GreaterThan:
                                stackPush(make_shared<GI::BooleanObject>(leftValue > rightValue));
                                break;
                            default:
                                // unreachable
                                break;
                        }
                    } else {
                        if (left->getType() == GI::ObjectType::BOOLEAN && right->getType() == GI::ObjectType::BOOLEAN) {
                            auto leftValue = static_cast<GI::BooleanObject *>(left.get())->value;
                            auto rightValue = static_cast<GI::BooleanObject *>(right.get())->value;
                            switch (opCode) {
                                case OpCode::Equal:
                                    stackPush(make_shared<GI::BooleanObject>(leftValue == rightValue));
                                    break;
                                case OpCode::NotEqual:
                                    stackPush(make_shared<GI::BooleanObject>(leftValue != rightValue));
                                    break;
                                case OpCode::GreaterThan:
                                    throw "unsupported greater than instruction on bool";
                                default:
                                    // unreachable
                                    break;
                            }
                        } else {
                            throw "unsupported infix operation on type: " + objectTypeMapping.map[left->getType()];
                        }

                    }
                    break;
                }
                case OpCode::Bang: {
                    auto operand = stackPop();
                    if (operand->getType() == GI::ObjectType::BOOLEAN) {
                        auto boolObject = static_cast<GI::BooleanObject *>(operand.get());
                        stackPush(make_shared<GI::BooleanObject>(!boolObject->value));
                    } else {
                        stackPush(make_shared<GI::BooleanObject>(false));
                    }

                    break;
                }
                case OpCode::Minus: {
                    auto operand = stackPop();
                    if (operand->getType() != GI::ObjectType::INTEGER) {
                        throw "unsupported type for negation: " + objectTypeMapping.map[operand->getType()];
                    }
                    auto integerObject = static_cast<GI::IntegerObject *>(operand.get());
                    stackPush(make_shared<GI::IntegerObject>(-integerObject->value));
                    break;
                }
                case OpCode::Pop:
                    stackPop();
                    break;
                default:
                    throw "unsupported instruction on vm: " + to_string(to_integer<int>(instruction));
            }
        }

    }


    void VM::stackPush(shared_ptr<GI::GIObject> object) {
        if (sp >= STACK_SIZE) {
            throw "Stack overflow";
        }
        if (stack.size() == sp) {
            stack.push_back(object);
        } else {
            stack[sp] = object;
        }
        sp++;
    }


    shared_ptr<GI::GIObject> VM::stackPop() {
        auto object = stack[sp - 1];
        sp--;
        return object;
    }
}

#pragma clang diagnostic pop
