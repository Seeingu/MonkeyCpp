//
// Created by seeu on 2022/7/9.
//

#ifndef GOINTERPRETER_PARSER_H
#define GOINTERPRETER_PARSER_H

#include <string>
#include <utility>
#include <vector>
#include "Token.h"
#include "Lexer.h"
#include "Ast.h"

namespace GI {
    using namespace std;
    enum class Precedence {
        LOWEST = 0,
        EQUALS = 1,
        LESS = 2,
        SUM = 3,
        PRODUCT = 4,
        PREFIX = 5,
        CALL = 6,
        INDEX = 7,
    };

    class ParserError {
    public:
        ParserError(Token token, std::string message) : token{std::move(token)}, message{std::move(message)} {};

        std::string toString();

    private:
        Token token;
        std::string message;

    };

    class Parser {
    public:
        explicit Parser(Lexer *lexer)
                : lexer(lexer) {

            precedences[TokenType::EQ] = Precedence::EQUALS;
            precedences[TokenType::NOT_EQ] = Precedence::EQUALS;
            precedences[TokenType::LT] = Precedence::LESS;
            precedences[TokenType::GT] = Precedence::LESS;
            precedences[TokenType::PLUS] = Precedence::SUM;
            precedences[TokenType::MINUS] = Precedence::SUM;
            precedences[TokenType::SLASH] = Precedence::PRODUCT;
            precedences[TokenType::ASTERISK] = Precedence::PRODUCT;
            precedences[TokenType::LPAREN] = Precedence::CALL;
            precedences[TokenType::LBRACKET] = Precedence::INDEX;
        }

        std::unique_ptr<Program> parseProgram();

    private:

        std::unique_ptr<Statement> parseStatement();

        std::unique_ptr<LetStatement> parseLetStatement();

        std::unique_ptr<ReturnStatement> parseReturnStatement();

        std::unique_ptr<ExpressionStatement> parseExpressionStatement();

        std::unique_ptr<BlockStatement> parseBlockStatement();

        bool expectPeekAndConsume(TokenType tokenType);

        void peekMatchError(TokenType tokenType);

        Precedence peekPrecedence();

        std::unique_ptr<Expression> parseExpression(Precedence precedence);

        std::unique_ptr<IntegerExpression> parseIntegerExpression();

        std::unique_ptr<StringExpression> parseStringExpression();

        std::unique_ptr<PrefixExpression> parsePrefixExpression();

        std::unique_ptr<BoolExpression> parseBoolExpression();

        std::unique_ptr<IfExpression> parseIfExpression();

        std::unique_ptr<Expression> parseGroupedExpression();

        std::unique_ptr<ArrayExpression> parseArrayExpression();

        std::unique_ptr<HashExpression> parseHashExpression();

        std::unique_ptr<FunctionExpression> parseFunctionExpression();

        std::vector<std::unique_ptr<Identifier>> parseFunctionParameters();

        std::unique_ptr<InfixExpression> parseInfixExpression(std::unique_ptr<Expression> left);

        std::unique_ptr<IndexExpression> parseIndexExpression(std::unique_ptr<Expression> left);

        std::unique_ptr<CallExpression> parseCallExpression(std::unique_ptr<Expression> left);

        std::vector<std::unique_ptr<Expression>> parseCallArguments();

        void nextToken();

        Lexer *lexer;
        Token currentToken{TokenType::START, ""};
        Token peekToken{TokenType::START, ""};
        std::vector<std::unique_ptr<ParserError>> errors;
        std::map<TokenType, Precedence> precedences;

        TokenTypeMapping tokenTypeMapping;

    };
}

#endif //GOINTERPRETER_PARSER_H
