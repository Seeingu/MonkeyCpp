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

    class Compiler {
    public:
        Compiler() {}

        void compile(GI::Node *node);

        Instruction instructions;
        vector <shared_ptr<GI::GIObject>> constants;

    private:
        int emit(OpCode opCode, vector<int> operands = {});

        int addInstruction(Instruction instruction);

        int addConstant(shared_ptr<GI::GIObject> object);

        Code code{};
    };
}


#endif //GOINTERPRETER_COMPILER_H
