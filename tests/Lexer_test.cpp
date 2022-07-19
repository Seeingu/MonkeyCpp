//
// Created by seeu on 2022/7/3.
//
#include "catch2/catch_all.hpp"
#include <iostream>
#include "Lexer.h"

TEST_CASE("Lexer", "[lexer]" ) {
    const char* input = R"""(
    let five = 5;
    let ten = 10;

    let add = fn(x, y) {
      x + y;
    };

    let result = add(five, ten);
    !-/*5;
    5 < 10 > 5;

    if (5 < 10) {
        return true;
    } else {
        return false;
    }

    10 == 10;
    10 != 9;
                 )""";

    std::pair<GI::TokenType, std::string> tokens[74] =  {
        {std::make_pair(GI::TokenType::LET, "let")},
        {std::make_pair(GI::TokenType::IDENT, "five")},
        {std::make_pair(GI::TokenType::ASSIGN, "=")},
        {std::make_pair(GI::TokenType::INT, "5")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::LET, "let")},
        {std::make_pair(GI::TokenType::IDENT, "ten")},
        {std::make_pair(GI::TokenType::ASSIGN, "=")},
        {std::make_pair(GI::TokenType::INT, "10")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::LET, "let")},
        {std::make_pair(GI::TokenType::IDENT, "add")},
        {std::make_pair(GI::TokenType::ASSIGN, "=")},
        {std::make_pair(GI::TokenType::FUNCTION, "fn")},
        {std::make_pair(GI::TokenType::LPAREN, "(")},
        {std::make_pair(GI::TokenType::IDENT, "x")},
        {std::make_pair(GI::TokenType::COMMA, ",")},
        {std::make_pair(GI::TokenType::IDENT, "y")},
        {std::make_pair(GI::TokenType::RPAREN, ")")},
        {std::make_pair(GI::TokenType::LBRACE, "{")},
        {std::make_pair(GI::TokenType::IDENT, "x")},
        {std::make_pair(GI::TokenType::PLUS, "+")},
        {std::make_pair(GI::TokenType::IDENT, "y")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::RBRACE, "}")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::LET, "let")},
        {std::make_pair(GI::TokenType::IDENT, "result")},
        {std::make_pair(GI::TokenType::ASSIGN, "=")},
        {std::make_pair(GI::TokenType::IDENT, "add")},
        {std::make_pair(GI::TokenType::LPAREN, "(")},
        {std::make_pair(GI::TokenType::IDENT, "five")},
        {std::make_pair(GI::TokenType::COMMA, ",")},
        {std::make_pair(GI::TokenType::IDENT, "ten")},
        {std::make_pair(GI::TokenType::RPAREN, ")")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::BANG, "!")},
        {std::make_pair(GI::TokenType::MINUS, "-")},
        {std::make_pair(GI::TokenType::SLASH, "/")},
        {std::make_pair(GI::TokenType::ASTERISK, "*")},
        {std::make_pair(GI::TokenType::INT, "5")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::INT, "5")},
        {std::make_pair(GI::TokenType::LT, "<")},
        {std::make_pair(GI::TokenType::INT, "10")},
        {std::make_pair(GI::TokenType::GT, ">")},
        {std::make_pair(GI::TokenType::INT, "5")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::IF, "if")},
        {std::make_pair(GI::TokenType::LPAREN, "(")},
        {std::make_pair(GI::TokenType::INT, "5")},
        {std::make_pair(GI::TokenType::LT, "<")},
        {std::make_pair(GI::TokenType::INT, "10")},
        {std::make_pair(GI::TokenType::RPAREN, ")")},
        {std::make_pair(GI::TokenType::LBRACE, "{")},
        {std::make_pair(GI::TokenType::RETURN, "return")},
        {std::make_pair(GI::TokenType::TRUE, "true")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::RBRACE, "}")},
        {std::make_pair(GI::TokenType::ELSE, "else")},
        {std::make_pair(GI::TokenType::LBRACE, "{")},
        {std::make_pair(GI::TokenType::RETURN, "return")},
        {std::make_pair(GI::TokenType::FALSE, "false")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::RBRACE, "}")},
        {std::make_pair(GI::TokenType::INT, "10")},
        {std::make_pair(GI::TokenType::EQ, "==")},
        {std::make_pair(GI::TokenType::INT, "10")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::INT, "10")},
        {std::make_pair(GI::TokenType::NOT_EQ, "!=")},
        {std::make_pair(GI::TokenType::INT, "9")},
        {std::make_pair(GI::TokenType::SEMICOLON, ";")},
        {std::make_pair(GI::TokenType::_EOF, "")},
    };

    GI::TokenTypeMapping mapping;
    auto nameMapping = mapping.map;

    GI::Lexer lexer{input};
    for (int i = 0; i < 74; i++) {
        auto token = tokens[i];
        auto t = lexer.nextToken();
//        std::cout << "Index: " << i << std::endl;
        REQUIRE(nameMapping[t.type] == nameMapping[token.first]);
        REQUIRE(t.literal == token.second);
    }
}
