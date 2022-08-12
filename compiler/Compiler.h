//
// Created by seeu on 2022/8/9.
//

#ifndef GOINTERPRETER_COMPILER_H
#define GOINTERPRETER_COMPILER_H

#include <vector>
#include "Ast.h"
#include "Code.h"
#include "GIObject.h"

namespace GC {
    using namespace std;

    struct EmittedInstruction {
        OpCode code;
        int position;
    };

    class Compiler {
    public:
        Compiler() {}

        void compile(Common::Node *node);

        Instruction instructions;
        vector<shared_ptr<Common::GIObject>> constants;

    private:
        int emit(OpCode opCode, vector<int> operands = {});

        void setLastInstruction(OpCode code, int position);

        void changeOperand(int position, vector<int> operand);

        int addInstruction(Instruction instruction);

        int addConstant(shared_ptr<Common::GIObject> object);

        EmittedInstruction lastInstruction{};
        EmittedInstruction previousInstruction{};

        Code code{};
    };
}


#endif //GOINTERPRETER_COMPILER_H
