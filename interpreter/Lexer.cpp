//
// Created by seeu on 2022/7/3.
//

#include "Lexer.h"
#include <sstream>
#include <string>

namespace GI {

    std::map<std::string, TokenType> initializeKeywords() {
        std::map<std::string, TokenType> keywordsMap;
        keywordsMap["fn"] = TokenType::FUNCTION;
        keywordsMap["let"] = TokenType::LET;
        keywordsMap["true"] = TokenType::TRUE;
        keywordsMap["false"] = TokenType::FALSE;
        keywordsMap["if"] = TokenType::IF;
        keywordsMap["else"] = TokenType::ELSE;
        keywordsMap["return"] = TokenType::RETURN;
        return keywordsMap;
    }


    Lexer::Lexer(std::string input) : input{std::move(input)}, readPosition{0}, position{0}, currentChar{0} {
        keywordsMap = initializeKeywords();
    }

    std::string charToString(char c) {
        std::string s;
        s = c;
        return s;
    }

    void Lexer::skipWhitespace() {
        while (currentChar == ' ' || currentChar == '\t' || currentChar == '\n' || currentChar == '\r') {
            readChar();
        }
    }

    bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    bool isLetter(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    std::string Lexer::readNumber() {
        auto pos = position;
        while (isDigit(peekChar())) {
            readChar();
        }

        // 1: peek ahead
        return input.substr(pos, position + 1 - pos);
    }

    std::string Lexer::readIdentifier() {
        auto pos = position;
        while (isLetter(peekChar())) {
            readChar();
        }
        // 1: peek ahead
        return input.substr(pos, position + 1 - pos);
    }

    unsigned int Lexer::peekChar() {
        if (readPosition >= input.size()) {
            return 0;
        } else {
            return input[readPosition];
        }

    }

    void Lexer::readChar() {
        if (readPosition >= input.size()) {
            currentChar = 0;
        } else {
            currentChar = input[readPosition];
        }
        position = readPosition;
        readPosition += 1;
    }

    Token Lexer::nextToken() {
        readChar();
        skipWhitespace();

        switch (currentChar) {
            case '=':
                if (peekChar() == '=') {
                    auto ch = currentChar;
                    // sign
                    readChar();
                    std::stringstream literal;
                    literal << ch;
                    literal << currentChar;
                    return {TokenType::EQ, literal.str()};
                } else {
                    return {TokenType::ASSIGN, charToString(currentChar)};
                }
            case '+':
                return {TokenType::PLUS, charToString(currentChar)};
            case '-':
                return {TokenType::MINUS, charToString(currentChar)};
            case '!':
                if (peekChar() == '=') {
                    auto ch = currentChar;
                    readChar();
                    std::stringstream literal;
                    literal << ch;
                    literal << currentChar;
                    return {TokenType::NOT_EQ, literal.str()};
                } else {
                    return {TokenType::BANG, charToString(currentChar)};
                }
            case '/':
                return {TokenType::SLASH, charToString(currentChar)};
            case '*':
                return {TokenType::ASTERISK, charToString(currentChar)};
            case '<':
                return {TokenType::LT, charToString(currentChar)};
            case '>':
                return {TokenType::GT, charToString(currentChar)};
            case ';':
                return {TokenType::SEMICOLON, charToString(currentChar)};
            case ',':
                return {TokenType::COMMA, charToString(currentChar)};
            case '{':
                return {TokenType::LBRACE, charToString(currentChar)};
            case '}':
                return {TokenType::RBRACE, charToString(currentChar)};
            case '(':
                return {TokenType::LPAREN, charToString(currentChar)};
            case ')':
                return {TokenType::RPAREN, charToString(currentChar)};
            case 0:
                return {TokenType::_EOF, ""};
            default:
                if (isLetter(currentChar)) {
                    auto literal = readIdentifier();
                    auto c = keywordsMap.size();
                    auto type = keywordsMap.contains(literal) ? keywordsMap[literal] : TokenType::IDENT;
                    return {type, literal};
                } else if (isDigit(currentChar)) {
                    return {TokenType::INT, readNumber()};
                } else {
                    return {TokenType::ILLEGAL, charToString(currentChar)};
                }
        }
    }
}
