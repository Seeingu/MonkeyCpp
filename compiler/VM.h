//
// Created by seeu on 2022/8/9.
//

#ifndef GOINTERPRETER_VM_H
#define GOINTERPRETER_VM_H

#include <vector>
#include "GIObject.h"
#include "Code.h"

#define STACK_SIZE 1024

namespace GC {
    using namespace std;

    class VM {
    public:
        VM(vector<shared_ptr<GI::GIObject>> constants, Instruction instructions) : constants{std::move(constants)},
                                                                                   instructions{
                                                                                           std::move(instructions)} {
            stack.reserve(STACK_SIZE);
        }

        void run();

        shared_ptr<GI::GIObject> stackTop();

    private:
        vector<shared_ptr<GI::GIObject>> constants;

        void stackPush(shared_ptr<GI::GIObject> object);

        shared_ptr<GI::GIObject> stackPop();

        Instruction instructions;
        vector<shared_ptr<GI::GIObject>> stack;
        int sp{0};
        Code code{};
    };
}


#endif //GOINTERPRETER_VM_H
