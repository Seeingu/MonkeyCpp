//
// Created by seeu on 2022/8/12.
//

#ifndef GOINTERPRETER_COMPILEROBJECT_H
#define GOINTERPRETER_COMPILEROBJECT_H

#include "GIObject.h"
#include "Code.h"

using namespace Common;
namespace GC {
    struct CompiledFunctionObject : GIObject {
        CompiledFunctionObject(const Instruction &instructions, int numParameters, int numLocals) : numLocals(
                numLocals), numParameters(numParameters), instructions(instructions) {}

        ObjectType getType() override {
            return ObjectType::COMPILED_FUNCTION;
        }

        string inspect() override {
            return "";
        }

        Instruction instructions;
        int numParameters;
        int numLocals;
    };

}


#endif //GOINTERPRETER_COMPILEROBJECT_H
