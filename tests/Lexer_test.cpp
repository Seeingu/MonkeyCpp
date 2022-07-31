//
// Created by seeu on 2022/7/3.
//
#include "catch2/catch_all.hpp"
#include <iostream>
#include "Lexer.h"

TEST_CASE("Lexer", "[lexer]") {
    const char *input = R"""(
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
    2 * (5 + 10);
    "foobar"
    "foo bar"
                 )""";

    struct Expected {
        GI::TokenType type;
        std::string value;
    };
    std::vector<Expected> tokens = {
            {GI::TokenType::LET,        "let"},
            {GI::TokenType::IDENTIFIER, "five"},
            {GI::TokenType::ASSIGN,     "="},
            {GI::TokenType::INT,        "5"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::LET,        "let"},
            {GI::TokenType::IDENTIFIER, "ten"},
            {GI::TokenType::ASSIGN,     "="},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::LET,        "let"},
            {GI::TokenType::IDENTIFIER, "add"},
            {GI::TokenType::ASSIGN,     "="},
            {GI::TokenType::FUNCTION,   "fn"},
            {GI::TokenType::LPAREN,     "("},
            {GI::TokenType::IDENTIFIER, "x"},
            {GI::TokenType::COMMA,      ","},
            {GI::TokenType::IDENTIFIER, "y"},
            {GI::TokenType::RPAREN,     ")"},
            {GI::TokenType::LBRACE,     "{"},
            {GI::TokenType::IDENTIFIER, "x"},
            {GI::TokenType::PLUS,       "+"},
            {GI::TokenType::IDENTIFIER, "y"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::RBRACE,     "}"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::LET,        "let"},
            {GI::TokenType::IDENTIFIER, "result"},
            {GI::TokenType::ASSIGN,     "="},
            {GI::TokenType::IDENTIFIER, "add"},
            {GI::TokenType::LPAREN,     "("},
            {GI::TokenType::IDENTIFIER, "five"},
            {GI::TokenType::COMMA,      ","},
            {GI::TokenType::IDENTIFIER, "ten"},
            {GI::TokenType::RPAREN,     ")"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::BANG,       "!"},
            {GI::TokenType::MINUS,      "-"},
            {GI::TokenType::SLASH,      "/"},
            {GI::TokenType::ASTERISK,   "*"},
            {GI::TokenType::INT,        "5"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::INT,        "5"},
            {GI::TokenType::LT,         "<"},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::GT,         ">"},
            {GI::TokenType::INT,        "5"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::IF,         "if"},
            {GI::TokenType::LPAREN,     "("},
            {GI::TokenType::INT,        "5"},
            {GI::TokenType::LT,         "<"},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::RPAREN,     ")"},
            {GI::TokenType::LBRACE,     "{"},
            {GI::TokenType::RETURN,     "return"},
            {GI::TokenType::TRUE,       "true"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::RBRACE,     "}"},
            {GI::TokenType::ELSE,       "else"},
            {GI::TokenType::LBRACE,     "{"},
            {GI::TokenType::RETURN,     "return"},
            {GI::TokenType::FALSE,      "false"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::RBRACE,     "}"},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::EQ,         "=="},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::NOT_EQ,     "!="},
            {GI::TokenType::INT,        "9"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::INT,        "2"},
            {GI::TokenType::ASTERISK,   "*"},
            {GI::TokenType::LPAREN,     "("},
            {GI::TokenType::INT,        "5"},
            {GI::TokenType::PLUS,       "+"},
            {GI::TokenType::INT,        "10"},
            {GI::TokenType::RPAREN,     ")"},
            {GI::TokenType::SEMICOLON,  ";"},
            {GI::TokenType::STRING,     "foobar"},
            {GI::TokenType::STRING,     "foo bar"},
            {GI::TokenType::_EOF,       ""},
    };

    GI::TokenTypeMapping mapping;
    auto nameMapping = mapping.map;

    GI::Lexer lexer{input};
    for (auto &token: tokens) {
        auto t = lexer.nextToken();
//        std::cout << "Index: " << i << std::endl;
        REQUIRE(nameMapping[t.type] == nameMapping[token.type]);
        REQUIRE(t.literal == token.value);
    }
}
