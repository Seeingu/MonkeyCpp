//
// Created by seeu on 2022/7/3.
//

#ifndef GOINTERPRETER_TOKEN_H
#define GOINTERPRETER_TOKEN_H

#include <string>
#include <map>
#include <utility>


namespace Common {
    enum class TokenType {
        START = -2,
        ILLEGAL = -1,
        _EOF,

        IDENTIFIER,
        INT,
        STRING,

        ASSIGN,
        PLUS,
        MINUS,
        BANG,
        ASTERISK,
        SLASH,

        LT,
        GT,

        EQ,
        NOT_EQ,

        COMMA,
        SEMICOLON,
        COLON,

        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET,
        RBRACKET,

        FUNCTION,
        LET,
        TRUE,
        FALSE,
        IF,
        ELSE,
        RETURN,
    };

    class Token {
    public:
        TokenType type;
        std::string literal;

        Token(TokenType type, std::string literal) : type{type}, literal{std::move(literal)} {}
    };


}

#endif //GOINTERPRETER_TOKEN_H
