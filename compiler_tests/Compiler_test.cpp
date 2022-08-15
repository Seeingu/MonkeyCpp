#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//
// Created by seeu on 2022/8/9.
//

#include <vector>
#include <memory>
#include <iostream>
#include <variant>
#include "catch2/catch_all.hpp"
#include "Code.h"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "CompilerObject.h"

using namespace std;

TEST_CASE("test compile", "[compiler]") {
    struct TestCase {
        string input;
        vector<variant<int, string>> expectedConstants;
        vector<GC::Instruction> expectedInstructions;
    };

    GC::Code code{};

    vector<TestCase> cases = {
            {"1 + 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Add),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {"1 - 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Sub),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "1 < 2",
                    {2,     1},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::GreaterThan),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "1 > 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(
                                    GC::OpCode::GreaterThan),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},

            {
             "1 * 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Mul),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "1 / 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Div),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "1; 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Pop),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "1 == 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Equal),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "1 != 2",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::NotEqual),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "-2",
                    {2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Minus),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "!true",
                    {},
                    {
                            code.makeInstruction(GC::OpCode::True),
                            code.makeInstruction(GC::OpCode::Bang),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "if (true) { 10 }; 3333;",
                    {10,    3333},
                    {
                            // 0000
                            code.makeInstruction(GC::OpCode::True),
                            // 0001
                            code.makeInstruction(GC::OpCode::JumpNotTruthy, {10}),
                            // 0004
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            // 0007
                            code.makeInstruction(GC::OpCode::Jump, {11}),
                            // 0010
                            code.makeInstruction(GC::OpCode::_Null),
                            // 0011
                            code.makeInstruction(GC::OpCode::Pop),
                            // 0012
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            // 0015
                            code.makeInstruction(GC::OpCode::Pop)
                    }},
            {
             "if (true) { 10 } else { 20 }; 3333;",
                    {10,    20, 3333},
                    {
                            // 0000
                            code.makeInstruction(GC::OpCode::True),
                            // 0001
                            code.makeInstruction(GC::OpCode::JumpNotTruthy, {10}),
                            // 0004
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            // 0007
                            code.makeInstruction(GC::OpCode::Jump, {13}),
                            // 0010
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            // 0013
                            code.makeInstruction(GC::OpCode::Pop),
                            // 0014
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            // 0017
                            code.makeInstruction(GC::OpCode::Pop)
                    }},
            {
             "let one = 1; let two = 2;",
                    {1,     2},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {1}),
                    }},
            {
             R"(let one = 1;
			let two = one;
			two;)",
                    {1},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::GetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {1}),
                            code.makeInstruction(GC::OpCode::GetGlobal, {1}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }},
            {
             R"("monkey")",
                    {"monkey"},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
             R"("mon" + "key")",
                    {"mon", "key"},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Add),
                            code.makeInstruction(GC::OpCode::Pop),
                    }
            },
            {
             "[]",
                    {},
                    {
                            code.makeInstruction(GC::OpCode::Array, {0}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
             "[1, 2, 3]",
                    {1,     2,  3},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Array, {3}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
             "[1 + 2, 3 - 4, 5 * 6]",
                    {1,     2,  3, 4, 5, 6},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Add),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Constant, {3}),
                            code.makeInstruction(GC::OpCode::Sub),
                            code.makeInstruction(GC::OpCode::Constant, {4}),
                            code.makeInstruction(GC::OpCode::Constant, {5}),
                            code.makeInstruction(GC::OpCode::Mul),
                            code.makeInstruction(GC::OpCode::Array, {3}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },

            {
             "{}",
                    {},
                    {
                            code.makeInstruction(GC::OpCode::Hash, {0}),
                            code.makeInstruction(GC::OpCode::Pop),
                    }
            },
            {
             "{1: 2, 3: 4, 5: 6}",
                    {1,     2,  3, 4, 5, 6},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Constant, {3}),
                            code.makeInstruction(GC::OpCode::Constant, {4}),
                            code.makeInstruction(GC::OpCode::Constant, {5}),
                            code.makeInstruction(GC::OpCode::Hash, {6}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
             "{1: 2 + 3, 4: 5 - 6}",
                    {1,     2,  3, 4, 5, 6},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Add),
                            code.makeInstruction(GC::OpCode::Constant, {3}),
                            code.makeInstruction(GC::OpCode::Constant, {4}),
                            code.makeInstruction(GC::OpCode::Constant, {5}),
                            code.makeInstruction(GC::OpCode::Sub),
                            code.makeInstruction(GC::OpCode::Hash, {4}),
                            code.makeInstruction(GC::OpCode::Pop)

                    }
            },

            {"[1, 2, 3][1 + 1]",
                    {1,     2,  3, 1, 1},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Array, {3}),
                            code.makeInstruction(GC::OpCode::Constant, {3}),
                            code.makeInstruction(GC::OpCode::Constant, {4}),
                            code.makeInstruction(GC::OpCode::Add),
                            code.makeInstruction(GC::OpCode::Index),
                            code.makeInstruction(GC::OpCode::Pop),
                    }},
            {
             "{1: 2}[2 - 1]",
                    {1,     2,  2, 1},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Hash, {2}),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Constant, {3}),
                            code.makeInstruction(GC::OpCode::Sub),
                            code.makeInstruction(GC::OpCode::Index),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
             "len([]);",
                    {},
                    {
                            code.makeInstruction(GC::OpCode::GetBuiltin, {0}),
                            code.makeInstruction(GC::OpCode::Array, {0}),
                            code.makeInstruction(GC::OpCode::Call, {1}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            }

    };

    for (auto &testCase: cases) {
        Common::Lexer lexer{testCase.input};
        Common::Parser parser{&lexer};
        auto program = parser.parseProgram();

        GC::Compiler compiler;
        compiler.compile(program.get());

        GC::Instruction ins;
        for (auto &instruction: testCase.expectedInstructions) {
            ins.insert(ins.end(), instruction.begin(), instruction.end());
        }
        REQUIRE(ins == compiler.getByteCode().instructions);
        for (int i = 0; i < compiler.constants.size(); i++) {
            if (compiler.constants[i]->getType() == Common::ObjectType::INTEGER) {
                auto value = static_cast<Common::IntegerObject *>(compiler.constants[i].get())->value;
                REQUIRE(value == std::get<int>(testCase.expectedConstants[i]));
            } else if (compiler.constants[i]->getType() == Common::ObjectType::STRING) {
                auto value = static_cast<Common::StringObject *>(compiler.constants[i].get())->value;
                REQUIRE(value == std::get<string>(testCase.expectedConstants[i]));
            }
        }

    }
}

TEST_CASE("compile function", "[compiler]") {
    struct TestCase {
        string input;
        vector<variant<int, vector<GC::Instruction>>> expectedConstants;
        vector<GC::Instruction> expectedInstructions;
    };

    GC::Code code{};

    vector<TestCase> cases = {
            {
                    "fn() {}",
                    {
                            vector<GC::Instruction>{
                                    code.makeInstruction(GC::OpCode::Return)
                            },
                    },
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    "fn() { 5 + 10 }",
                    {       5, 10,
                                   vector<GC::Instruction>{
                                           code.makeInstruction(GC::OpCode::Constant, {0}),
                                           code.makeInstruction(GC::OpCode::Constant, {1}),
                                           code.makeInstruction(GC::OpCode::Add),
                                           code.makeInstruction(GC::OpCode::ReturnValue),
                                   }
                    },
                    {
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    R"(let noArg = fn() { 24 };noArg();)",
                    {       24,
                               vector<GC::Instruction>{
                                       code.makeInstruction(GC::OpCode::Constant, {0}),
                                       code.makeInstruction(GC::OpCode::ReturnValue),
                               },
                    },
                    {
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::GetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::Call, {0}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    R"(let oneArg = fn(a) { a };oneArg(12);)",
                    {
                            vector<GC::Instruction>{
                                    code.makeInstruction(GC::OpCode::GetLocal, {0}),
                                    code.makeInstruction(GC::OpCode::ReturnValue)
                            },
                               12},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::GetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Call, {1}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    R"(let manyArg = fn(a, b, c) { a; b; c }; manyArg(10, 11, 12);)",
                    {
                            vector<GC::Instruction>{
                                    code.makeInstruction(GC::OpCode::GetLocal, {0}),
                                    code.makeInstruction(GC::OpCode::Pop),
                                    code.makeInstruction(GC::OpCode::GetLocal, {1}),
                                    code.makeInstruction(GC::OpCode::Pop),
                                    code.makeInstruction(GC::OpCode::GetLocal, {2}),
                                    code.makeInstruction(GC::OpCode::ReturnValue),
                            },
                               10, 11, 12},
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::GetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Constant, {2}),
                            code.makeInstruction(GC::OpCode::Constant, {3}),
                            code.makeInstruction(GC::OpCode::Call, {3}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    R"(let num = 55;fn() { num })",
                    {       55,
                               vector<GC::Instruction>{
                                       code.makeInstruction(GC::OpCode::GetGlobal, {0}),
                                       code.makeInstruction(GC::OpCode::ReturnValue)
                               },
                    },
                    {
                            code.makeInstruction(GC::OpCode::Constant, {0}),
                            code.makeInstruction(GC::OpCode::SetGlobal, {0}),
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    R"(fn() {let num = 55;num})",
                    {       55,
                               vector<GC::Instruction>{
                                       code.makeInstruction(GC::OpCode::Constant, {0}),
                                       code.makeInstruction(GC::OpCode::SetLocal, {0}),
                                       code.makeInstruction(GC::OpCode::GetLocal, {0}),
                                       code.makeInstruction(GC::OpCode::ReturnValue)
                               },
                    },
                    {
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
            {
                    "fn() { return 1; }",
                    {       1,
                               vector<GC::Instruction>{
                                       code.makeInstruction(GC::OpCode::Constant, {0}),
                                       code.makeInstruction(GC::OpCode::ReturnValue)
                               },
                    },
                    {
                            code.makeInstruction(GC::OpCode::Constant, {1}),
                            code.makeInstruction(GC::OpCode::Pop)
                    }
            },
    };

    for (auto &testCase: cases) {
        Common::Lexer lexer{testCase.input};
        Common::Parser parser{&lexer};
        auto program = parser.parseProgram();

        GC::Compiler compiler;
        compiler.compile(program.get());

        GC::Instruction ins;
        for (auto &instruction: testCase.expectedInstructions) {
            ins.insert(ins.end(), instruction.begin(), instruction.end());
        }
        REQUIRE(ins == compiler.getByteCode().instructions);
        for (int i = 0; i < compiler.constants.size(); i++) {
            if (compiler.constants[i]->getType() == Common::ObjectType::INTEGER) {
                auto value = static_cast<Common::IntegerObject *>(compiler.constants[i].get())->value;
                REQUIRE(value == std::get<int>(testCase.expectedConstants[i]));
            } else if (compiler.constants[i]->getType() == Common::ObjectType::COMPILED_FUNCTION) {
                auto functionObject = static_cast<GC::CompiledFunctionObject *>(compiler.constants[i].get());
                GC::Instruction fnIns;
                auto instructions = std::get<vector<GC::Instruction>>(testCase.expectedConstants[i]);
                for (auto &instruction: instructions) {
                    fnIns.insert(fnIns.end(), instruction.begin(), instruction.end());
                }
                REQUIRE(functionObject->instructions == fnIns);
            }
        }

    }
}

#pragma clang diagnostic pop
