//
// Created by seeu on 2022/8/9.
//

#include <vector>
#include <memory>
#include <cstddef>
#include "catch2/catch_all.hpp"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "VM.h"

using namespace std;

TEST_CASE("vm test", "[vm]") {
    struct TestCase {
        string input;
        int expected;
    };

    vector<TestCase> cases = {
            {"1",     1},
            {"1 + 2", 3}
    };

    for (auto &testCase: cases) {
        GI::Lexer lexer{testCase.input};
        GI::Parser parser{&lexer};
        auto program = parser.parseProgram();

        GC::Compiler compiler;
        compiler.compile(program.get());
        GC::VM vm{std::move(compiler.constants), compiler.instructions};
        vm.run();

        REQUIRE(vm.stackTop()->getType() == GI::ObjectType::INTEGER);
        auto integerObject = static_cast<GI::IntegerObject *>(vm.stackTop().get());
        REQUIRE(integerObject->value == testCase.expected);
    }

}
