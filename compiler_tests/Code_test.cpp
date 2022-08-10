//
// Created by seeu on 2022/8/8.
//

#include <vector>
#include <memory>
#include <cstddef>
#include "catch2/catch_all.hpp"
#include "Code.h"

using namespace std;

TEST_CASE("code make instruction", "[code]") {
    struct TestCase {
        GC::OpCode op;
        std::vector<int> operands;
        std::vector<std::byte> expected;
    };
    vector<TestCase> cases = {
            {GC::OpCode::Constant, vector<int>{65534},
             std::vector<byte>{byte{GC::OpCode::Constant}, byte{255}, byte{254}}},
    };

    GC::Code code{};
    for (auto &testCase: cases) {
        auto result = code.makeInstruction(testCase.op, testCase.operands);
        REQUIRE(result == testCase.expected);
    }

}

TEST_CASE("instruction to string", "[code]") {
    struct TestCase {
        GC::Instruction instruction;
        string expected;
    };
    GC::Code code{};
    vector<GC::Instruction> instructions = {
            code.makeInstruction(GC::OpCode::Add),
            code.makeInstruction(GC::OpCode::Constant, {2}),
            code.makeInstruction(GC::OpCode::Constant, {65535}),
    };
    auto expected = R"""(0000 OpAdd
0001 OpConstant 2
0004 OpConstant 65535
)""";
    GC::Instruction ins;
    for (auto &instruction: instructions) {
        ins.insert(ins.end(), instruction.begin(), instruction.end());
    }
    REQUIRE(code.instructionToString(ins) == expected);

}
