//
// Created by seeu on 2022/7/3.
//

#ifndef GOINTERPRETER_LEXER_H
#define GOINTERPRETER_LEXER_H

#include <string>
#include <utility>
#include "Token.h"

namespace Common {

    class Lexer {
    public:
        explicit Lexer(std::string input);

        Token nextToken();

    private:
        std::string input;
        std::map<std::string, TokenType> keywordsMap;
        unsigned position{};
        unsigned readPosition;
        char currentChar{};

        void skipWhitespace();

        void readChar();


        unsigned int peekChar();

        std::string readIdentifier();

        std::string readNumber();

        std::string readString();

    };
}

#endif //GOINTERPRETER_LEXER_H
