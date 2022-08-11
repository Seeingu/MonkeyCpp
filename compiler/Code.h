//
// Created by seeu on 2022/8/8.
//

#ifndef GOINTERPRETER_CODE_H
#define GOINTERPRETER_CODE_H

#include <vector>
#include <string>
#include <map>
#include <cstddef>

#define OP_DEF(name) definitions.emplace(GC::OpCode::name, GC::Definition{ \
        #name, \
        vector<int>{} \
});

namespace GC {
    using namespace std;

    using Instruction = std::vector<byte>;

    enum class OpCode : std::underlying_type_t<std::byte> {
        Constant = 0,
        Add,
        Sub,
        Mul,
        Div,
        True,
        False,
        Pop,
        Equal,
        NotEqual,
        GreaterThan,
        Minus,
        Bang
    };


    struct Definition {
        string name;
        std::vector<int> operandWidths;
    };

    class Code {
    public:
        Code() {
            definitions.emplace(OpCode::Constant, Definition{
                    "Constant",
                    vector<int>{2}
            });
            OP_DEF(Add);
            OP_DEF(Sub);
            OP_DEF(Mul);
            OP_DEF(Div);
            OP_DEF(Pop);
            OP_DEF(True);
            OP_DEF(False);
            OP_DEF(Equal);
            OP_DEF(NotEqual);
            OP_DEF(GreaterThan);
            OP_DEF(Minus);
            OP_DEF(Bang);
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
