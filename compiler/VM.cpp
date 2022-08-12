#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//
// Created by seeu on 2022/8/9.
//

#include "VM.h"
#include "magic_enum.hpp"
#include "fmt/format.h"
#include <cstddef>
#include <string>


namespace GC {
    bool isTruthy(shared_ptr<Common::GIObject> object) {
        if (object->getType() == Common::ObjectType::BOOLEAN) {
            return static_cast<Common::BooleanObject *>(object.get())->value;
        } else if (object->getType() == Common::ObjectType::_NULL) {
            return false;
        }
        return true;
    }

    shared_ptr<Common::GIObject> VM::stackTop() {
        if (sp == 0) {
            return nullptr;
        }
        return stack[sp - 1];
    }

    shared_ptr<Common::GIObject> VM::lastStackElem() {
        return stack[sp];
    }

    void VM::run() {
        for (int i = 0; i < instructions.size(); i++) {
            auto instruction = instructions[i];
            auto opCode = OpCode(instruction);
            switch (opCode) {
                case OpCode::Constant: {
                    int constIndex = readUint16(i + 1);
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
                    if (left->getType() == Common::ObjectType::INTEGER &&
                        right->getType() == Common::ObjectType::INTEGER) {
                        auto leftValue = static_cast<Common::IntegerObject *>(left.get())->value;
                        auto rightValue = static_cast<Common::IntegerObject *>(right.get())->value;

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
                        stackPush(make_shared<Common::IntegerObject>(result));
                    } else if (left->getType() == Common::ObjectType::STRING &&
                               right->getType() == Common::ObjectType::STRING) {
                        auto leftValue = static_cast<Common::StringObject *>(left.get())->value;
                        auto rightValue = static_cast<Common::StringObject *>(right.get())->value;
                        if (opCode == OpCode::Add) {
                            stackPush(make_shared<Common::StringObject>(leftValue + rightValue));
                        } else {
                            throw fmt::format("unsupported binary operation {} on string",
                                              to_string(int(opCode)));
                        }
                    }
                    break;
                }
                case OpCode::True:
                case OpCode::False:
                    stackPush(make_shared<Common::BooleanObject>(opCode == OpCode::True));
                    break;
                case OpCode::Equal:
                case OpCode::NotEqual:
                case OpCode::GreaterThan: {
                    auto right = stackPop();
                    auto left = stackPop();
                    if (left->getType() == Common::ObjectType::INTEGER &&
                        right->getType() == Common::ObjectType::INTEGER) {
                        auto leftValue = static_cast<Common::IntegerObject *>(left.get())->value;
                        auto rightValue = static_cast<Common::IntegerObject *>(right.get())->value;
                        switch (opCode) {
                            case OpCode::Equal:
                                stackPush(make_shared<Common::BooleanObject>(leftValue == rightValue));
                                break;
                            case OpCode::NotEqual:
                                stackPush(make_shared<Common::BooleanObject>(leftValue != rightValue));
                                break;
                            case OpCode::GreaterThan:
                                stackPush(make_shared<Common::BooleanObject>(leftValue > rightValue));
                                break;
                            default:
                                // unreachable
                                break;
                        }
                    } else {
                        if (left->getType() == Common::ObjectType::BOOLEAN &&
                            right->getType() == Common::ObjectType::BOOLEAN) {
                            auto leftValue = static_cast<Common::BooleanObject *>(left.get())->value;
                            auto rightValue = static_cast<Common::BooleanObject *>(right.get())->value;
                            switch (opCode) {
                                case OpCode::Equal:
                                    stackPush(make_shared<Common::BooleanObject>(leftValue == rightValue));
                                    break;
                                case OpCode::NotEqual:
                                    stackPush(make_shared<Common::BooleanObject>(leftValue != rightValue));
                                    break;
                                case OpCode::GreaterThan:
                                    throw "unsupported greater than instruction on bool";
                                default:
                                    // unreachable
                                    break;
                            }
                        } else {
                            throw fmt::format("unsupported infix operation on type: {}",
                                              magic_enum::enum_name(left->getType()));
                        }

                    }
                    break;
                }
                case OpCode::Bang: {
                    auto operand = stackPop();
                    if (operand->getType() == Common::ObjectType::BOOLEAN) {
                        auto boolObject = static_cast<Common::BooleanObject *>(operand.get());
                        stackPush(make_shared<Common::BooleanObject>(!boolObject->value));
                    } else {
                        stackPush(make_shared<Common::BooleanObject>(false));
                    }

                    break;
                }
                case OpCode::Minus: {
                    auto operand = stackPop();
                    if (operand->getType() != Common::ObjectType::INTEGER) {
                        throw fmt::format("unsupported type for negation: {}",
                                          magic_enum::enum_name(operand->getType()));
                    }
                    auto integerObject = static_cast<Common::IntegerObject *>(operand.get());
                    stackPush(make_shared<Common::IntegerObject>(-integerObject->value));
                    break;
                }
                case OpCode::Pop:
                    stackPop();
                    break;
                case OpCode::Jump: {
                    auto insIndex = readUint16(i + 1);
                    i = insIndex - 1;
                    break;
                }
                case OpCode::JumpNotTruthy: {
                    auto insIndex = readUint16(i + 1);
                    i += 2;

                    auto condition = stackPop();
                    if (!isTruthy(condition)) {
                        i = insIndex - 1;
                    }
                    break;
                }
                case OpCode::_Null: {
                    stackPush(make_shared<Common::NullObject>());
                    break;
                }
                case OpCode::SetGlobal: {
                    auto globalIndex = readUint16(i + 1);
                    i += 2;

                    if (globals.size() == globalIndex) {
                        globals.push_back(stackPop());
                    } else {
                        globals[globalIndex] = stackPop();
                    }

                    break;
                }
                case OpCode::GetGlobal: {
                    auto globalIndex = readUint16(i + 1);
                    i += 2;

                    stackPush(globals[globalIndex]);
                    break;
                }
                default:
                    throw "unsupported instruction on vm: " + to_string(to_integer<int>(instruction));
            }
        }

    }


    void VM::stackPush(shared_ptr<Common::GIObject> object) {
        if (sp >= STACK_SIZE) {
            throw "Stack overflow";
        }
        // TODO: use intuitive actions
        if (stack.size() == sp) {
            stack.push_back(object);
        } else {
            stack[sp] = object;
        }
        sp++;
    }


    shared_ptr<Common::GIObject> VM::stackPop() {
        auto object = stack[sp - 1];
        sp--;
        return object;
    }


    int VM::readUint16(int index) {
        Instruction ins;
        ins.assign(instructions.begin() + index, instructions.end());
        return code.readUint16(ins);
    }
}

#pragma clang diagnostic pop
