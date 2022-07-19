//
// Created by seeu on 2022/7/16.
//

#include "catch2/catch_all.hpp"
#include "Lexer.h"
#include "Parser.h"
#include "Evaluator.h"
#include <iostream>
#include <string>
#include <memory>

using namespace GI;

TEST_CASE("eval int expression", "[evaluator]") {
    struct TestCase {
        std::string input;
        int expected;
    };
    std::vector<TestCase> cases = {
            {"5",                               5},
            {"10",                              10},
            {"-5",                              -5},
            {"-10",                             -10},
            {"5 + 5 + 5 + 5 - 10",              10},
            {"2 * 2 * 2 * 2 * 2",               32},
            {"-50 + 100 + -50",                 0},
            {"5 * 2 + 10",                      20},
            {"5 + 2 * 10",                      25},
            {"20 + 2 * -10",                    0},
            {"50 / 2 * 2 + 10",                 60},
            {"2 * (5 + 10)",                    30},
            {"3 * 3 * 3 + 10",                  37},
            {"3 * (3 * 3) + 10",                37},
            {"(5 + 10 * 2 + 15 / 3) * 2 + -10", 50},
    };
    for (auto &testCase: cases) {
        Lexer lexer{testCase.input};
        std::cout << "Test Input: " << testCase.input << std::endl;
        Parser parser{&lexer};
        auto program = parser.parseProgram();
        auto env = std::make_shared<Environment>();
        auto result = eval(program.get(), env);
        auto integerObject = dynamic_cast<IntegerObject *>(result.get());
        REQUIRE(integerObject->value == testCase.expected);
    }
}
