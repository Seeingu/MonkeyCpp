//
// Created by seeu on 2022/8/9.
//

#include <vector>
#include <memory>
#include "catch2/catch_all.hpp"
#include "Code.h"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"

using namespace std;

TEST_CASE("compile", "[compiler]") {
    struct TestCase {
        string input;
        vector<int> expectedConstants;
        vector<GC::Instruction> expectedInstructions;
    };

    GC::Code code{};


    vector<TestCase> cases = {
            {"1 + 2", vector<int>{1, 2}, vector<GC::Instruction>{
                    code.makeInstruction(GC::OpCode::Constant, {0}),
                    code.makeInstruction(GC::OpCode::Constant, {1}),
                    code.makeInstruction(GC::OpCode::Add)
            }}
    };

    for (auto &testCase: cases) {
        GI::Lexer lexer{testCase.input};
        GI::Parser parser{&lexer};
        auto program = parser.parseProgram();

        GC::Compiler compiler;
        compiler.compile(program.get());

        GC::Instruction ins;
        for (auto &instruction: testCase.expectedInstructions) {
            ins.insert(ins.end(), instruction.begin(), instruction.end());
        }
        REQUIRE(ins == compiler.instructions);
        for (int i = 0; i < compiler.constants.size(); i++) {
            auto value = static_cast<GI::IntegerObject *>(compiler.constants[i].get())->value;
            REQUIRE(value == testCase.expectedConstants[i]);
        }

    }
}
