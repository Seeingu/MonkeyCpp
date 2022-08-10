//
// Created by seeu on 2022/8/8.
//

#include "Code.h"
#include <numeric>
#include <cstddef>
#include <sstream>
#include <string>
#include <iomanip>
#include <tuple>
#include "fmt/core.h"

namespace GC {
    string formatInstruction(Definition *definition, vector<int> operands) {
        if (definition->operandWidths.size() != operands.size()) {
            return "Error: operand length not matches: " + definition->name;
        }
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
        int instructionLength = 1;
        auto definition = definitions[code];
        instructionLength += std::accumulate(definition.operandWidths.begin(), definition.operandWidths.end(), 0);

        auto instruction = Instruction{};
        instruction.push_back(byte{code});

        for (int o: operands) {
            auto width = definition.operandWidths[0];
            if (width == 2) {
                // little endian
                instruction.push_back(byte(o >> 8));
                instruction.push_back(byte(o & 0xff));
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
                int op = to_integer<int>(instruction[offset]) << 8;
                op = op | to_integer<int>(instruction[offset + 1]);
                operands.push_back(op);
            }
            offset += operandWidth;
        }
        return make_pair(operands, offset);
    }

}
