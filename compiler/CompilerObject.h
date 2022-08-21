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
                numLocals), numParameters{numParameters}, instructions{instructions} {}

        ObjectType getType() override {
            return ObjectType::COMPILED_FUNCTION;
        }

        string inspect() override {
            return "CompiledFunction";
        }

        Instruction instructions;
        int numParameters;
        int numLocals;
    };

    struct ClosureObject : GIObject {
        ClosureObject(CompiledFunctionObject compiledFunctionObject, std::vector<shared_ptr<GIObject>> freeObjects)
                : compiledFunctionObject(std::move(compiledFunctionObject)), freeObjects(std::move(freeObjects)) {}

        CompiledFunctionObject compiledFunctionObject;
        std::vector<shared_ptr<GIObject>> freeObjects;

        ObjectType getType() override { return Common::ObjectType::CLOSURE; }

        std::string inspect() override { return "Closure"; }
    };
}


#endif //GOINTERPRETER_COMPILEROBJECT_H
