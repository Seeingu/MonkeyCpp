//
// Created by seeu on 2022/7/9.
//
#include <vector>
#include <memory>
#include <algorithm>
#include "catch2/catch_all.hpp"
#include "Ast.h"

using namespace GI;

TEST_CASE("Ast", "[ast]") {
    SECTION("let expression") {
        std::vector<std::unique_ptr<Statement>> stmts{};
        auto stmt = std::make_unique<LetStatement>(Token(TokenType::LET, "let"),
                                                   std::make_unique<Identifier>(
                                                           Token{TokenType::IDENT, "myVar"}, "myVar"),
                                                   std::make_unique<Identifier>(
                                                           Token{TokenType::IDENT, "anotherVar"},
                                                           "anotherVar"));
        stmts.push_back(std::move(stmt));
        auto program = Program{std::move(stmts)};
        REQUIRE(program.toString() == "let myVar = anotherVar;");
    }
}
