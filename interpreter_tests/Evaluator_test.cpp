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
#include <utility>

using namespace std;
using namespace Common;
using namespace GI;

std::shared_ptr<GIObject> testEval(std::string input) {
    Lexer lexer{std::move(input)};
    Parser parser{&lexer};
    auto program = parser.parseProgram();
    auto env = std::make_shared<Environment>();
    return eval(program.get(), env);
}

template<typename T, typename E>
void testExpression(std::string input, E expected) {
    auto result = testEval(std::move(input));
    auto object = dynamic_cast<T *>(result.get());
    REQUIRE(object->value == expected);
}

TEST_CASE("eval", "[evaluator]") {
    auto input = R"(
    let five = 5;
    let ten = 10;

    let add = fn(x, y) {
      x + y;
    };

    let result = add(five, ten);
    result;
    )";
    auto result = testEval(input);
    result->inspect();
}

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
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }
}

TEST_CASE("eval boolean expression", "[evaluator]") {
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> cases = {
            {"true",             true},
            {"false",            false},
            {"1 < 2",            true},
            {"1 > 2",            false},
            {"1 < 1",            false},
            {"1 > 1",            false},
            {"1 == 1",           true},
            {"1 != 1",           false},
            {"1 == 2",           false},
            {"1 != 2",           true},
            {"true == true",     true},
            {"false == false",   true},
            {"true == false",    false},
            {"true != false",    true},
            {"false != true",    true},
            {"(1 < 2) == true",  true},
            {"(1 < 2) == false", false},
            {"(1 > 2) == true",  false},
            {"(1 > 2) == false", true},
    };

    for (auto &testCase: cases) {
        testExpression<BooleanObject>(testCase.input, testCase.expected);
    }
}


TEST_CASE("eval string expression", "[evaluator]") {
    struct TestCase {
        std::string input;
        std::string expected;
    };
    std::vector<TestCase> cases = {
            {"\"value\"",             "value"},
            {R"("Hello " + "World")", "Hello World"},
    };
    for (auto &testCase: cases) {
        testExpression<StringObject>(testCase.input, testCase.expected);
    }
}

TEST_CASE("eval array expression", "[evaluator]") {
    string input{"[1, 2 * 2, 3 + 3, -4]"};
    auto result = testEval(input);
    auto object = static_cast<ArrayObject *>(result.get());
    REQUIRE(object->elements.size() == 4);
    REQUIRE(static_cast<IntegerObject *>(object->elements[0].get())->value == 1);
    REQUIRE(static_cast<IntegerObject *>(object->elements[1].get())->value == 4);
    REQUIRE(static_cast<IntegerObject *>(object->elements[2].get())->value == 6);
    REQUIRE(static_cast<IntegerObject *>(object->elements[3].get())->value == -4);
}

TEST_CASE("eval array index", "[evaluator]") {
    struct TestCase {
        string input;
        int expected;
    };
    vector<TestCase> cases = {
            {
                    "[1, 2, 3][0]",
                    1,
            },
            {
                    "[1, 2, 3][1]",
                    2,
            },
            {
                    "[1, 2, 3][2]",
                    3,
            },
            {
                    "let i = 0; [1][i];",
                    1,
            },
            {
                    "[1, 2, 3][1 + 1];",
                    3,
            },
            {
                    "let myArray = [1, 2, 3]; myArray[2];",
                    3,
            },
            {
                    "let myArray = [1, 2, 3]; myArray[0] + myArray[1] + myArray[2];",
                    6,
            },
            {
                    "let myArray = [1, 2, 3]; let i = myArray[0]; myArray[i]",
                    2,
            },
    };
    for (auto &testCase: cases) {
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }

}

TEST_CASE("eval array index overflow", "[evaluator]") {
    struct TestCase {
        string input;
    };
    vector<TestCase> cases = {
            {
                    "[1, 2, 3][3]",
            },
            {
                    "[1, 2, 3][-1]",
            },
    };
    for (auto &testCase: cases) {
        auto result = testEval(testCase.input);
        REQUIRE(result == nullptr);
    }

}

TEST_CASE("hashmap", "[evaluator]") {
    auto input = R"(
    let two = "two";
	{
		"one": 10 - 9,
		two: 1 + 1,
		"thr" + "ee": 6 / 2,
		4: 4,
		true: 5,
		false: 6
	}
    )";
    auto result = testEval(input);
    REQUIRE(result->getType() == Common::ObjectType::HASH);

    auto hashObject = static_cast<HashObject *>(result.get());
    REQUIRE(hashObject->pairs.size() == 6);
    auto firstHash = static_cast<IntegerObject *>(hashObject->pairs[StringObject{"one"}.hash()].value.get());
    REQUIRE(firstHash->value == 1);
}

TEST_CASE("hashmap index", "[evaluator]") {
    struct TestCase {
        string input;
        int expected;
    };
    vector<TestCase> cases = {
            {R"({"foo": 5}["foo"])",                5},
            {R"(let key = "foo"; {"foo": 5}[key])", 5}
    };
    for (auto &testCase: cases) {
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }
}

TEST_CASE("bang expression", "[evaluator]") {
    struct TestCase {
        std::string input;
        bool expected;
    };

    std::vector<TestCase> cases = {
            {"!true",        false},
            {"!false",       true},
            {"!0",           false},
            {"!!true",       true},
            {"!!false",      false},
            {"!!0",          true},
            {"!\"\"",        false},
            {"!!\"\"",       true},
            {"!\"string\"",  false},
            {"!!\"string\"", true}
    };

    for (auto &testCase: cases) {
        testExpression<BooleanObject>(testCase.input, testCase.expected);
    }
}

TEST_CASE("if expression", "[evaluator]") {
    struct TestCase {
        std::string input;
        std::optional<int> expected;
    };

    std::vector<TestCase> cases = {
            {"if (true) { 10 }",              10},
            {"if (false) { 10 }",             std::nullopt},
            {"if (1) { 10 }",                 10},
            {"if (1 < 2) { 10 }",             10},
            {"if (1 > 2) { 10 }",             std::nullopt},
            {"if (1 > 2) { 10 } else { 20 }", 20},
            {"if (1 < 2) { 10 } else { 20 }", 10},
    };

    for (auto &testCase: cases) {
        Lexer lexer{testCase.input};
        Parser parser{&lexer};
        // std::cout << testCase.input << std::endl;
        auto program = parser.parseProgram();
        auto env = std::make_shared<Environment>();
        auto result = eval(program.get(), env);
        auto intObject = dynamic_cast<IntegerObject *>(result.get());
        if (testCase.expected.has_value()) {
            REQUIRE(intObject->value == testCase.expected);
        }
    }
}

TEST_CASE("return statement", "[evaluator]") {
    struct TestCase {
        std::string input;
        int expected;
    };

    std::vector<TestCase> cases = {
            {"return 10;",                 10},
            {"return 10; 9;",              10},
            {"return 2 * 5; 9;",           10},
            {"9; return 2 * 5; 9;",        10},
            {"if (10 > 1) { return 10; }", 10},
            {
             R"(
                    if (10 > 1) {
                        if (10 > 1) {
                            return 10;
                        }

                        return 1;
                    }
                    )",
                                           10,
            },
            {
             R"(
                    let f = fn(x) {
                        return x;
                        x + 10;
                    };
                    f(10);)",
                                           10,
            },
            {
             R"(
                    let f = fn(x) {
                        let result = x + 10;
                        return result;
                        return 10;
                    };
                    f(10);)",
                                           20,
            },
    };

    for (auto &testCase: cases) {
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }
}

TEST_CASE("error handling", "[evaluator]") {
    struct TestCase {
        std::string input;
        std::string errorMessage;
    };
    std::vector<TestCase> cases = {
            {
                    "5 + true;",
                    "type mismatch: INTEGER + BOOLEAN",
            },
            {
                    "5 + true; 5;",
                    "type mismatch: INTEGER + BOOLEAN",
            },
            {
                    "-true",
                    "unknown operator: -BOOLEAN",
            },
            {
                    "true + false;",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "true + false + true + false;",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "5; true + false; 5",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "if (10 > 1) { true + false; }",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    R"""(
                    if (10 > 1) {
                        if (10 > 1) {
                            return true + false;
                        }

                        return 1;
                    }
                    )""",
                    "unknown operator: BOOLEAN + BOOLEAN",
            },
            {
                    "foobar",
                    "identifier not found: foobar",
            },
    };
    for (auto &testCase: cases) {
        Lexer lexer{testCase.input};
        Parser parser{&lexer};
        auto program = parser.parseProgram();
        auto env = std::make_shared<Environment>();
        auto result = eval(program.get(), env);
        auto errorObject = dynamic_cast<ErrorObject *>(result.get());
        REQUIRE(errorObject->message == testCase.errorMessage);
    }
}

TEST_CASE("let statement", "[evaluator]") {
    struct TestCase {
        std::string input;
        int expected;
    };
    std::vector<TestCase> cases = {
            {"let a = 5; a;",                               5},
            {"let a = 5 * 5; a;",                           25},
            {"let a = 5; let b = a; b;",                    5},
            {"let a = 5; let b = a; let c = a + b + 5; c;", 15},
    };
    for (auto &testCase: cases) {
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }
}

TEST_CASE("function", "[evaluator]") {
    struct TestCase {
        std::string input;
        int expected;
    };

    std::vector<TestCase> cases = {
            {"let identity = fn(x) { x; }; identity(5);",             5},
            {"let identity = fn(x) { return x; }; identity(5);",      5},
            {"let double = fn(x) { x * 2; }; double(5);",             10},
            {"let add = fn(x, y) { x + y; }; add(5, 5);",             10},
            {"let add = fn(x, y) { x + y; }; add(5 + 5, add(5, 5));", 20},
            {"fn(x) { x; }(5)",                                       5},
    };
    for (auto &testCase: cases) {
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }

    testExpression<StringObject>("fn(x) { x; }(\"hello\")", "hello");
}

TEST_CASE("enclosing env", "[evaluator]") {
    auto input = R"""(
    let first = 10;
    let second = 10;
    let third = 10;

    let ourFunction = fn(first) {
        let second = 20;

        first + second + third;
    };
    ourFunction(20) + first + second;
    )""";

    testExpression<IntegerObject>(input, 70);
}

TEST_CASE("builtin functions", "[evaluator]") {
    struct TestCase {
        std::string input;
        int expected;
    };

    std::vector<TestCase> cases = {
            {"len(\"hello\")",       5},
            {"len(\"hello world\")", 11},
            {"len(\"\")",            0},
    };
    for (auto &testCase: cases) {
        testExpression<IntegerObject>(testCase.input, testCase.expected);
    }
}
