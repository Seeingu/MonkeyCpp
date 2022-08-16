//
// Created by seeu on 2022/8/9.
//

#ifndef GOINTERPRETER_COMPILER_H
#define GOINTERPRETER_COMPILER_H

#include <vector>
#include "Ast.h"
#include "Code.h"
#include "SymbolTable.h"
#include "GIObject.h"

namespace GC {
    using namespace std;


    struct EmittedInstruction {
        OpCode code;
        int position;
    };
    struct CompilationScope {
        Instruction instructions;
        EmittedInstruction lastInstruction;
        EmittedInstruction previousInstruction;
    };

    using Constants = vector<shared_ptr<Common::GIObject>>;
    struct ByteCode {
        Instruction instructions;
        Constants constants;
    };

    class Compiler {
    public:
        Compiler() {
            scopes.push_back(
                    {
                            .instructions =  {},
                            .lastInstruction =  EmittedInstruction{},
                            .previousInstruction =  EmittedInstruction{}
                    });
            symbolTableManager.defineBuiltin(0, "len");
        }

        void compile(Common::Node *node);

        Constants constants;

        ByteCode getByteCode() {
            return {
                    currentInstructions(),
                    constants
            };
        }


    private:
        int emit(OpCode opCode, vector<int> operands = {});

        void setLastInstruction(OpCode code, int position);

        void changeOperand(int position, vector<int> operand);

        int addInstruction(Instruction instruction);

        int addConstant(shared_ptr<Common::GIObject> object);

        void replaceInstruction(int position, Instruction instruction);

        void replaceLastPopWithReturn();

        void enterScope();

        Instruction leaveScope();

        Instruction currentInstructions() {
            return scopes[scopeIndex].instructions;
        }

        Instruction *instructions() {
            return &scopes[scopeIndex].instructions;
        }

        EmittedInstruction lastInstruction() {
            return scopes[scopeIndex].lastInstruction;
        }

        EmittedInstruction previousInstruction() {
            return scopes[scopeIndex].previousInstruction;
        }

        void loadSymbol(Symbol symbol);

        SymbolTableManager symbolTableManager{};

        Code code{};

        int scopeIndex{0};
        vector<CompilationScope> scopes;
    };
}


#endif //GOINTERPRETER_COMPILER_H
