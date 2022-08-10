//
// Created by seeu on 2022/8/8.
//

#ifndef GOINTERPRETER_CODE_H
#define GOINTERPRETER_CODE_H

#include <vector>
#include <string>
#include <map>
#include <cstddef>

namespace GC {
    using namespace std;

    using Instruction = std::vector<byte>;

    enum class OpCode : std::underlying_type_t<std::byte> {
        Constant = 0,
        Add,
    };


    struct Definition {
        string name;
        std::vector<int> operandWidths;
    };

    class Code {
    public:
        Code() {
            definitions.emplace(OpCode::Constant, Definition{
                    .name = "OpConstant",
                    .operandWidths = vector<int>{2}
            });
            definitions.emplace(OpCode::Add, Definition{
                    .name = "OpAdd",
                    .operandWidths = vector<int>{}
            });
        }

        int readUint16(Instruction instruction) {
            int op = to_integer<int>(instruction[0]) << 8;
            op = op | to_integer<int>(instruction[1]);
            return op;
        }

        Instruction makeInstruction(OpCode code, std::vector<int> operands = {});

        string instructionToString(Instruction instruction);

        pair<vector<int>, int> readOperands(Definition *definition, Instruction instruction);

    private:
        std::map<OpCode, Definition> definitions;
    };
}

#endif //GOINTERPRETER_CODE_H
