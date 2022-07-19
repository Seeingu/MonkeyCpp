//
// Created by seeu on 2022/7/9.
//

#include "catch2/catch_all.hpp"
#include "Parser.h"
#include <string>
#include <type_traits>
#include <utility>
#include <variant>

using namespace GI;

static void testExpression(std::unique_ptr<Expression> expression, int value) {
    auto intExpr = dynamic_cast<IntegerExpression *>(expression.get());
    REQUIRE(intExpr->value == value);
}

static void testExpression(std::unique_ptr<Expression> expression, bool value) {
    auto boolExpr = dynamic_cast<BoolExpression *>(expression.get());
    REQUIRE(boolExpr->value == value);
}

static void testExpression(std::unique_ptr<Expression> expression, std::string value) {
    auto identifier = dynamic_cast<Identifier *>(expression.get());
    REQUIRE(identifier->value == value);
}


using ExpressionValue = std::variant<int, bool, std::string>;

TEST_CASE("let statement", "[parser]") {
    struct TestCase {
        std::string input;
        std::string name;
        ExpressionValue value;
    };
    std::vector<TestCase> cases{
            TestCase{"let x = 5;", "x", 5},
            TestCase{"let y = true;", "y", true},
            TestCase{"let foobar = y;", "foobar", "y"}
    };
    for (auto &testCase: cases) {
        Lexer lexer{std::move(testCase.input)};
        Parser parser{&lexer};
        auto program = parser.parseProgram();

        REQUIRE(program->statements.size() == 1);

        auto stmt = program->statements[0].get();

        auto letStatement = dynamic_cast<LetStatement *>(stmt);
        REQUIRE(letStatement->name->value == testCase.name);
        REQUIRE(letStatement->token.literal == "let");

        std::visit([&](auto v) {
            testExpression(std::move(letStatement->value), v);
        }, testCase.value);

    }
};

TEST_CASE("return statement", "[parser]") {
    struct TestCase {
        std::string input;
        ExpressionValue value;
    };
    std::vector<TestCase> cases{
            {"return 5;",      5},
            {"return true;",   true},
            {"return foobar;", "foobar"}
    };

    for (auto &testCase: cases) {
        Lexer lexer{std::move(testCase.input)};
        Parser parser{&lexer};
        auto program = parser.parseProgram();

        REQUIRE(program->statements.size() == 1);

        auto stmt = program->statements[0].get();

        auto returnStatement = dynamic_cast<ReturnStatement *>(stmt);
        REQUIRE(returnStatement->token.literal == "return");

        std::visit([&](auto v) {
            testExpression(std::move(returnStatement->returnValue), v);
        }, testCase.value);
    }

};

TEST_CASE("expression statement", "[parser]") {
    struct TestCase {
        std::string input;
        ExpressionValue value;
    };
    std::vector<TestCase> cases{
            {"foobar;", "foobar"},
            {"5;",      5},
            {"true;",   true}
    };
    for (auto &testCase: cases) {
        Lexer lexer{std::move(testCase.input)};
        Parser parser{&lexer};
        auto program = parser.parseProgram();

        REQUIRE(program->statements.size() == 1);

        auto stmt = program->statements[0].get();

        auto expressionStatement = dynamic_cast<ExpressionStatement *>(stmt);

        std::visit([&](auto v) {
            testExpression(std::move(expressionStatement->expression), v);
        }, testCase.value);
    }
};

TEST_CASE("prefix expression", "[parser]") {
    struct TestCase {
        std::string input;
        std::string prefix;
        ExpressionValue value;
    };
    std::vector<TestCase> cases{
            {"!5",      "!", 5},
            {"-15",     "-", 15},
            {"!foobar", "!", "foobar"},
            {"-foobar", "-", "foobar"},
            {"!true",   "!", true},
            {"!false",  "!", false}
    };

    for (auto &testCase: cases) {
        Lexer lexer{std::move(testCase.input)};
        Parser parser{&lexer};
        auto program = parser.parseProgram();

        REQUIRE(program->statements.size() == 1);

        auto stmt = program->statements[0].get();

        auto expressionStatement = dynamic_cast<ExpressionStatement *>(stmt);

        auto prefixExpression = dynamic_cast<PrefixExpression *>(expressionStatement->expression.get());
        REQUIRE(prefixExpression->prefixOperator == testCase.prefix);

        std::visit([&](auto v) {
            testExpression(std::move(prefixExpression->rightExpression), v);
        }, testCase.value);
    }

};

TEST_CASE("infix expression", "[parser]") {
    struct TestCase {
        std::string input;
        ExpressionValue left;
        std::string infix;
        ExpressionValue right;
    };
    std::vector<TestCase> cases{
            {"4 + 5;",            4,        "+",  5},
            {"5 - 5;",            5,        "-",  5},
            {"5 * 5;",            5,        "*",  5},
            {"5 / 5;",            5,        "/",  5},
            {"5 > 5;",            5,        ">",  5},
            {"5 < 5;",            5,        "<",  5},
            {"5 == 5;",           5,        "==", 5},
            {"5 != 5;",           5,        "!=", 5},
            {"foobar + barfoo;",  "foobar", "+",  "barfoo"},
            {"foobar - barfoo;",  "foobar", "-",  "barfoo"},
            {"foobar * barfoo;",  "foobar", "*",  "barfoo"},
            {"foobar / barfoo;",  "foobar", "/",  "barfoo"},
            {"foobar > barfoo;",  "foobar", ">",  "barfoo"},
            {"foobar < barfoo;",  "foobar", "<",  "barfoo"},
            {"foobar == barfoo;", "foobar", "==", "barfoo"},
            {"foobar != barfoo;", "foobar", "!=", "barfoo"},
            {"true == true",      true,     "==", true},
            {"true != false",     true,     "!=", false},
            {"false == false",    false,    "==", false},
    };

    for (auto &testCase: cases) {
        Lexer lexer{std::move(testCase.input)};
        Parser parser{&lexer};
        auto program = parser.parseProgram();

        REQUIRE(program->statements.size() == 1);

        auto stmt = program->statements[0].get();

        auto expressionStatement = dynamic_cast<ExpressionStatement *>(stmt);

        auto infixExpression = dynamic_cast<InfixExpression *>(expressionStatement->expression.get());
        REQUIRE(infixExpression->infixOperator == testCase.infix);

        std::visit([&](auto v) {
            testExpression(std::move(infixExpression->leftExpression), v);
        }, testCase.left);
        std::visit([&](auto v) {
            testExpression(std::move(infixExpression->rightExpression), v);
        }, testCase.right);

    }
};
