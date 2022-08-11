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
    [1, 2];
    {"foo": "bar"}
                 )""";

    struct Expected {
        Common::TokenType type;
        std::string value;
    };
    std::vector<Expected> tokens = {
            {Common::TokenType::LET,        "let"},
            {Common::TokenType::IDENTIFIER, "five"},
            {Common::TokenType::ASSIGN,     "="},
            {Common::TokenType::INT,        "5"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::LET,        "let"},
            {Common::TokenType::IDENTIFIER, "ten"},
            {Common::TokenType::ASSIGN,     "="},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::LET,        "let"},
            {Common::TokenType::IDENTIFIER, "add"},
            {Common::TokenType::ASSIGN,     "="},
            {Common::TokenType::FUNCTION,   "fn"},
            {Common::TokenType::LPAREN,     "("},
            {Common::TokenType::IDENTIFIER, "x"},
            {Common::TokenType::COMMA,      ","},
            {Common::TokenType::IDENTIFIER, "y"},
            {Common::TokenType::RPAREN,     ")"},
            {Common::TokenType::LBRACE,     "{"},
            {Common::TokenType::IDENTIFIER, "x"},
            {Common::TokenType::PLUS,       "+"},
            {Common::TokenType::IDENTIFIER, "y"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::RBRACE,     "}"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::LET,        "let"},
            {Common::TokenType::IDENTIFIER, "result"},
            {Common::TokenType::ASSIGN,     "="},
            {Common::TokenType::IDENTIFIER, "add"},
            {Common::TokenType::LPAREN,     "("},
            {Common::TokenType::IDENTIFIER, "five"},
            {Common::TokenType::COMMA,      ","},
            {Common::TokenType::IDENTIFIER, "ten"},
            {Common::TokenType::RPAREN,     ")"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::BANG,       "!"},
            {Common::TokenType::MINUS,      "-"},
            {Common::TokenType::SLASH,      "/"},
            {Common::TokenType::ASTERISK,   "*"},
            {Common::TokenType::INT,        "5"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::INT,        "5"},
            {Common::TokenType::LT,         "<"},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::GT,         ">"},
            {Common::TokenType::INT,        "5"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::IF,         "if"},
            {Common::TokenType::LPAREN,     "("},
            {Common::TokenType::INT,        "5"},
            {Common::TokenType::LT,         "<"},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::RPAREN,     ")"},
            {Common::TokenType::LBRACE,     "{"},
            {Common::TokenType::RETURN,     "return"},
            {Common::TokenType::TRUE,       "true"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::RBRACE,     "}"},
            {Common::TokenType::ELSE,       "else"},
            {Common::TokenType::LBRACE,     "{"},
            {Common::TokenType::RETURN,     "return"},
            {Common::TokenType::FALSE,      "false"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::RBRACE,     "}"},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::EQ,         "=="},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::NOT_EQ,     "!="},
            {Common::TokenType::INT,        "9"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::INT,        "2"},
            {Common::TokenType::ASTERISK,   "*"},
            {Common::TokenType::LPAREN,     "("},
            {Common::TokenType::INT,        "5"},
            {Common::TokenType::PLUS,       "+"},
            {Common::TokenType::INT,        "10"},
            {Common::TokenType::RPAREN,     ")"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::STRING,     "foobar"},
            {Common::TokenType::STRING,     "foo bar"},
            {Common::TokenType::LBRACKET,   "["},
            {Common::TokenType::INT,        "1"},
            {Common::TokenType::COMMA,      ","},
            {Common::TokenType::INT,        "2"},
            {Common::TokenType::RBRACKET,   "]"},
            {Common::TokenType::SEMICOLON,  ";"},
            {Common::TokenType::LBRACE,     "{"},
            {Common::TokenType::STRING,     "foo"},
            {Common::TokenType::COLON,      ":"},
            {Common::TokenType::STRING,     "bar"},
            {Common::TokenType::RBRACE,     "}"},
            {Common::TokenType::_EOF,       ""},
    };


    Common::Lexer lexer{input};
    for (auto &token: tokens) {
        auto t = lexer.nextToken();
//        std::cout << "Index: " << i << std::endl;
        REQUIRE(t.type == token.type);
        REQUIRE(t.literal == token.value);
    }
}
