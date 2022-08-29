//
// Created by seeu on 2022/8/8.
//

#include "Code.h"
#include <cstddef>
#include <sstream>
#include <string>
#include <tuple>
#include "fmt/core.h"

namespace GC {
    string formatInstruction(Definition *definition, vector<int> operands) {
        stringstream ss;
        ss << definition->name;
        for (auto &op: operands) {
            ss << " ";
            ss << to_string(op);
        }
        return ss.str();
    }

    Instruction Code::makeInstruction(OpCode code, std::vector<int> operands) {
        if (!definitions.contains(code)) {
            return {};
        }
        auto definition = definitions[code];

        auto instruction = Instruction{};
        instruction.push_back(byte{code});

        for (int i = 0; i < operands.size(); i++) {
            auto o = operands[i];
            auto width = definition.operandWidths[i];
            if (width == 2) {
                // little endian
                instruction.push_back(byte(o >> 8));
                instruction.push_back(byte(o & 0xff));
            } else if (width == 1) {
                instruction.push_back(byte(o));
            } else {
                throw "operand width size not matched: " + to_string(width);
            }
        }

        return instruction;
    }

    string Code::instructionToString(Instruction instruction) {
        stringstream ss;
        int i = 0;
        while (i < instruction.size()) {
            auto op = static_cast<OpCode>(instruction[i]);
            if (definitions.contains(op)) {
                ss << fmt::format("{:04}", i);
                Instruction ins = {instruction.begin() + i + 1, instruction.end()};
                auto pair = readOperands(&definitions[op], ins);

                ss << " " << formatInstruction(&definitions[op], pair.first);
                ss << endl;
                i += pair.second + 1;
            } else {
                ss << "Error: instruction not found in definition " << to_string(to_integer<int>(instruction[i]))
                   << endl;
                i++;
                continue;
            }

        }
        return ss.str();

    }

    pair<vector<int>, int> Code::readOperands(Definition *definition, Instruction instruction) {
        vector<int> operands;
        auto offset = 0;
        for (auto &operandWidth: definition->operandWidths) {
            if (operandWidth == 2) {
                Instruction ins{instruction.begin() + offset, instruction.end()};
                auto op = readUint16(ins);
                operands.push_back(op);
            } else if (operandWidth == 1) {
                Instruction ins{instruction.begin() + offset, instruction.end()};
                auto op = readUint8(ins);
                operands.push_back(op);
            }
            offset += operandWidth;
        }
        return make_pair(operands, offset);
    }

}
