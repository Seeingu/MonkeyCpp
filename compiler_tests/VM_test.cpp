//
// Created by seeu on 2022/8/9.
//

#include <vector>
#include <memory>
#include <variant>
#include "catch2/catch_all.hpp"
#include "Lexer.h"
#include "Parser.h"
#include "Compiler.h"
#include "VM.h"

using namespace std;

TEST_CASE("vm test", "[vm]") {
    struct TestCase {
        string input;
        variant<int, bool> expected;
    };

    vector<TestCase> cases = {
            // int
            {"1",                               1},
            {"2",                               2},
            {"1 + 2",                           3},
            {"1 - 2",                           -1},
            {"1 * 2",                           2},
            {"4 / 2",                           2},
            {"50 / 2 * 2 + 10 - 5",             55},
            {"5 * (2 + 10)",                    60},
            {"5 + 5 + 5 + 5 - 10",              10},
            {"2 * 2 * 2 * 2 * 2",               32},
            {"5 * 2 + 10",                      20},
            {"5 + 2 * 10",                      25},
            {"5 * (2 + 10)",                    60},
            {"-5",                              -5},
            {"-10",                             -10},
            {"-50 + 100 + -50",                 0},
            {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},

            // bool
            {"true",                            true},
            {"false",                           false},
            {"1 < 2",                           true},
            {"1 > 2",                           false},
            {"1 < 1",                           false},
            {"1 > 1",                           false},
            {"1 == 1",                          true},
            {"1 != 1",                          false},
            {"1 == 2",                          false},
            {"1 != 2",                          true},
            {"true == true",                    true},
            {"false == false",                  true},
            {"true == false",                   false},
            {"true != false",                   true},
            {"false != true",                   true},
            {"(1 < 2) == true",                 true},
            {"(1 < 2) == false",                false},
            {"(1 > 2) == true",                 false},
            {"(1 > 2) == false",                true},
            {"!true",                           false},
            {"!false",                          true},
            {"!5",                              false},
            {"!!true",                          true},
            {"!!false",                         false},
            {"!!5",                             true},
    };

    for (auto &testCase: cases) {
        GI::Lexer lexer{testCase.input};
        GI::Parser parser{&lexer};
        auto program = parser.parseProgram();

        GC::Compiler compiler;
        compiler.compile(program.get());
        GC::VM vm{std::move(compiler.constants), compiler.instructions};
        vm.run();

        if (vm.lastStackElem()->getType() == GI::ObjectType::INTEGER) {
            auto integerObject = static_cast<GI::IntegerObject *>(vm.lastStackElem().get());
            REQUIRE(integerObject->value == std::get<int>(testCase.expected));
        } else if (vm.lastStackElem()->getType() == GI::ObjectType::BOOLEAN) {
            auto boolObject = static_cast<GI::BooleanObject *>(vm.lastStackElem().get());
            if (boolObject->value) {
                REQUIRE(std::get<bool>(testCase.expected));
            } else {
                REQUIRE(boolObject->value == std::get<bool>(testCase.expected));
            }
        }
    }

}