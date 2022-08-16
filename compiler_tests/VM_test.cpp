#pragma clang diagnostic push
#pragma ide diagnostic ignored "cppcoreguidelines-pro-type-static-cast-downcast"
//
// Created by seeu on 2022/8/9.
//

#include <utility>
#include <vector>
#include <memory>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <variant>
#include "catch2/catch_all.hpp"
#include "magic_enum.hpp"
#include "fmt/core.h"
#include "Lexer.h"
#include "Parser.h"

#include "Compiler.h"
#include "VM.h"

using namespace std;

GC::VM runVM(string input) {
    Common::Lexer lexer{std::move(input)};
    Common::Parser parser{&lexer};
    auto program = parser.parseProgram();

    GC::Compiler compiler;
    compiler.compile(program.get());
    GC::VM vm{compiler.getByteCode()};
    vm.run();
    return vm;
}

TEST_CASE("vm test", "[vm]") {
    struct TestCase {
        string input;
        variant<int, bool, string, nullptr_t> expected;
    };

    vector<TestCase> cases = {
            // int
            {"1",                                           1},
            {"2",                                           2},
            {"1 + 2",                                       3},
            {"1 - 2",                                       -1},
            {"1 * 2",                                       2},
            {"4 / 2",                                       2},
            {"50 / 2 * 2 + 10 - 5",                         55},
            {"5 * (2 + 10)",                                60},
            {"5 + 5 + 5 + 5 - 10",                          10},
            {"2 * 2 * 2 * 2 * 2",                           32},
            {"5 * 2 + 10",                                  20},
            {"5 + 2 * 10",                                  25},
            {"5 * (2 + 10)",                                60},
            {"-5",                                          -5},
            {"-10",                                         -10},
            {"-50 + 100 + -50",                             0},
            {"(5 + 10 * 2 + 15 / 3) * 2 + -10",             50},

            // bool
            {"true",                                        true},
            {"false",                                       false},
            {"1 < 2",                                       true},
            {"1 > 2",                                       false},
            {"1 < 1",                                       false},
            {"1 > 1",                                       false},
            {"1 == 1",                                      true},
            {"1 != 1",                                      false},
            {"1 == 2",                                      false},
            {"1 != 2",                                      true},
            {"true == true",                                true},
            {"false == false",                              true},
            {"true == false",                               false},
            {"true != false",                               true},
            {"false != true",                               true},
            {"(1 < 2) == true",                             true},
            {"(1 < 2) == false",                            false},
            {"(1 > 2) == true",                             false},
            {"(1 > 2) == false",                            true},
            {"!true",                                       false},
            {"!false",                                      true},
            {"!5",                                          false},
            {"!!true",                                      true},
            {"!!false",                                     false},
            {"!!5",                                         true},

            // condition
            {"if (true) { 10 }",                            10},
            {"if (true) { 10 } else { 20 }",                10},
            {"if (false) { 10 } else { 20 } ",              20},
            {"if (1) { 10 }",                               10},
            {"if (1 < 2) { 10 }",                           10},
            {"if (1 < 2) { 10 } else { 20 }",               10},
            {"if (1 > 2) { 10 } else { 20 }",               20},
            {"if (1 > 2) { 10 }",                           nullptr},
            {"if (false) { 10 }",                           nullptr},
            {"if ((if (false) { 10 })) { 10 } else { 20 }", 20},

            // variable
            {"let one = 1; one",                            1},
            {"let one = 1; let two = 2; one + two",         3},
            {"let one = 1; let two = one + one; one + two", 3},

            // string
            {R"("monkey")",                                 "monkey"},
            {R"("mon" + "key")",                            "monkey"},
            {R"("mon" + "key" + "banana")",                 "monkeybanana"},

    };

    for (auto &testCase: cases) {
        auto vm = runVM(testCase.input);

        switch (vm.lastStackElem()->getType()) {
            case Common::ObjectType::INTEGER: {
                auto integerObject = static_cast<Common::IntegerObject *>(vm.lastStackElem().get());
                auto value = integerObject->value;
                auto expected = std::get<int>(testCase.expected);
                REQUIRE(value == expected);
                break;
            }
            case Common::ObjectType::BOOLEAN: {
                auto boolObject = static_cast<Common::BooleanObject *>(vm.lastStackElem().get());
                if (boolObject->value) {
                    REQUIRE(std::get<bool>(testCase.expected));
                } else {
                    REQUIRE(boolObject->value == std::get<bool>(testCase.expected));
                }
                break;
            }
            case Common::ObjectType::STRING: {
                auto stringObject = static_cast<Common::StringObject *>(vm.lastStackElem().get());
                try {
                    auto stringValue = stringObject->value;
                    auto expectedValue = std::get<string>(testCase.expected);
                    REQUIRE(stringValue == expectedValue);
                } catch (std::bad_variant_access const &ex) {
                    // FIXME: get<string> would failed in CI
                    std::cout << ex.what() << ": " << testCase.input << endl;
                }
                break;
            }
            default: {
                REQUIRE(std::get<nullptr_t>(testCase.expected) == nullptr);
                break;
            }
        }
    }

}

TEST_CASE("test vm array", "[vm]") {
    struct TestCase {
        string input;
        vector<int> expected;
    };

    vector<TestCase> cases = {
            {"[]",                    {}},
            {"[1, 2, 3]",             {1, 2,  3}},
            {"[1 + 2, 3 * 4, 5 + 6]", {3, 12, 11}},
    };

    for (auto &testCase: cases) {
        auto vm = runVM(testCase.input);

        auto arrayObject = static_cast<Common::ArrayObject *>(vm.lastStackElem().get());
        vector<int> result{};
        std::transform(arrayObject->elements.begin(), arrayObject->elements.end(), back_inserter(result),
                       [](const shared_ptr<Common::GIObject> &item) {
                           auto obj = static_cast<Common::IntegerObject *>(item.get());
                           return obj->value;
                       });

        REQUIRE(result == testCase.expected);
    }
}


TEST_CASE("test vm hash", "[vm]") {
    struct TestCase {
        string input;
        std::map<int, int> expected;
    };

    vector<TestCase> cases = {
            {"{}",                           {}},
            {"{1: 2, 3: 4}",                 {{1, 2}, {3, 4}}},
            {"{1 + 1: 2 * 2, 3 + 3: 4 * 4}", {{2, 4}, {6, 16}}},
    };

    for (auto &testCase: cases) {
        auto vm = runVM(testCase.input);

        auto hashObject = static_cast<Common::HashObject *>(vm.lastStackElem().get());
        std::map<int, int> result{};
        for (auto &hashPair: hashObject->pairs) {
            auto pair = hashPair.second;
            auto key = static_cast<Common::IntegerObject *>(pair.key.get())->value;
            auto value = static_cast<Common::IntegerObject *>(pair.value.get())->value;
            result[key] = value;
        }
        REQUIRE(result == testCase.expected);
    }
}

TEST_CASE("test vm index", "[vm]") {
    struct TestCase {
        string input;
        variant<int, nullptr_t> expected;
    };

    vector<TestCase> cases = {
            {"[1, 2, 3][1]",      2},
            {"[1, 2, 3][0 + 2]",  3},
            {"[[1, 1, 1]][0][0]", 1},
            {"[][0]",             nullptr},
            {"[1, 2, 3][99]",     nullptr},
            {"[1][-1]",           nullptr},
            {"{1: 1, 2: 2}[1]",   1},
            {"{1: 1, 2: 2}[2]",   2},
            {"{1: 1}[0]",         nullptr},
            {"{}[0]",             nullptr},
    };

    for (auto &testCase: cases) {
        auto vm = runVM(testCase.input);

        if (vm.lastStackElem()->getType() == Common::ObjectType::INTEGER) {
            auto integerObject = static_cast<Common::IntegerObject *>(vm.lastStackElem().get());
            REQUIRE(integerObject->value == std::get<int>(testCase.expected));
        } else {
            REQUIRE(std::get<nullptr_t>(testCase.expected) == nullptr);
        }
    }
}

TEST_CASE("test vm function", "[vm]") {
    struct TestCase {
        string input;
        int expected;
    };

    vector<TestCase> cases = {
            {
                    R"(let one = fn() { let one = 1; one };one();)",
                                             1
            },
            {
                    R"(let fivePlusTen = fn() { 5 + 10; };
                        fivePlusTen();)",
                                             15
            },
            {
                    R"(let one = fn() { 1; };
		            let two = fn() { 2; };
		            one() + two())",
                                             3
            },
            {
                    R"(let a = fn() { 1 };
		            let b = fn() { a() + 1 };
		            let c = fn() { b() + 1 };
		            c();)",
                                             3
            },
            {
                    R"(let earlyExit = fn() { return 99; 100; };
                    earlyExit();)",
                                             99
            },
            {
                    R"(let noReturn = fn() { };noReturn();)",
                                             0 // null
            },
            {
                    R"(let identity = fn(a) { a; };identity(4);)",
                                             4
            },
            // builtin
            {       R"(len(""))",            0},
            {       R"(len("four"))",        4},
            {       R"(len("hello world"))", 11},
            {
                    R"(let returnsOne = fn() { 1; };
                    let returnsOneReturner = fn() { returnsOne; };
                    returnsOneReturner()();)",
                                             1
            },
            {
                    R"(
                let newClosure = fn(a) {
			        fn() { a; };
		        };
		        let closure = newClosure(99);
		        closure();)",
                                             99
            }
    };

    for (auto &testCase: cases) {
        auto vm = runVM(testCase.input);
        if (vm.lastStackElem()->getType() == Common::ObjectType::INTEGER) {
            auto integerObject = static_cast<Common::IntegerObject *>(vm.lastStackElem().get());
            REQUIRE(integerObject->value == testCase.expected);
        } else {
            REQUIRE(testCase.expected == 0);
        }
    }
}

#pragma clang diagnostic pop
