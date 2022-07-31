//
// Created by seeu on 2022/7/3.
//

#ifndef GOINTERPRETER_TOKEN_H
#define GOINTERPRETER_TOKEN_H

#include <string>
#include <map>
#include <utility>

#define TOKEN_TYPE_MAP(name) map[GI::TokenType::name] = #name

namespace GI {
    enum class TokenType {
        START = -2,
        ILLEGAL = -1,
        _EOF,

// Identifiers + literals
        IDENTIFIER, // add, foobar, x, y, ...
        INT,
        STRING,

// Operators
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

// Delimiters
        COMMA,
        SEMICOLON,

        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,

// Keywords
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

    class TokenTypeMapping {
    public:
        TokenTypeMapping() {

            TOKEN_TYPE_MAP(ILLEGAL);
            TOKEN_TYPE_MAP(_EOF);
            TOKEN_TYPE_MAP(IDENTIFIER);
            TOKEN_TYPE_MAP(INT);
            TOKEN_TYPE_MAP(STRING);
            TOKEN_TYPE_MAP(ASSIGN);
            TOKEN_TYPE_MAP(PLUS);
            TOKEN_TYPE_MAP(MINUS);
            TOKEN_TYPE_MAP(BANG);
            TOKEN_TYPE_MAP(ASTERISK);
            TOKEN_TYPE_MAP(SLASH);
            TOKEN_TYPE_MAP(LT);
            TOKEN_TYPE_MAP(GT);
            TOKEN_TYPE_MAP(EQ);
            TOKEN_TYPE_MAP(NOT_EQ);
            TOKEN_TYPE_MAP(COMMA);
            TOKEN_TYPE_MAP(SEMICOLON);
            TOKEN_TYPE_MAP(LPAREN);
            TOKEN_TYPE_MAP(RPAREN);
            TOKEN_TYPE_MAP(LBRACE);
            TOKEN_TYPE_MAP(RBRACE);
            TOKEN_TYPE_MAP(FUNCTION);
            TOKEN_TYPE_MAP(LET);
            TOKEN_TYPE_MAP(TRUE);
            TOKEN_TYPE_MAP(FALSE);
            TOKEN_TYPE_MAP(IF);
            TOKEN_TYPE_MAP(ELSE);
            TOKEN_TYPE_MAP(RETURN);

        }

        std::map<GI::TokenType, std::string> map;
    };

}

#endif //GOINTERPRETER_TOKEN_H
