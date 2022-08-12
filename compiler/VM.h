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
        VM(vector <shared_ptr<Common::GIObject>> constants,
           Instruction instructions
        ) : constants{std::move(constants)},
            instructions{std::move(instructions)} {
            stack.reserve(STACK_SIZE);
        }

        void run();

        shared_ptr<Common::GIObject> stackTop();

        shared_ptr<Common::GIObject> lastStackElem();


    private:
        void stackPush(shared_ptr<Common::GIObject> object);

        shared_ptr<Common::GIObject> stackPop();

        int readUint16(int index);

        vector <shared_ptr<Common::GIObject>> constants;

        Instruction instructions;
        vector <shared_ptr<Common::GIObject>> stack;
        int sp{0};
        Code code{};

        vector <shared_ptr<Common::GIObject>> globals;
    };
}


#endif //GOINTERPRETER_VM_H
