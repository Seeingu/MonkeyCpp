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
                case OpCode::Add: {
                    auto right = stackPop();
                    auto left = stackPop();

                    auto result = static_cast<GI::IntegerObject *>(right.get())->value +
                                  static_cast<GI::IntegerObject *>(left.get())->value;
                    stackPush(make_shared<GI::IntegerObject>(result));
                    break;
                }
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
