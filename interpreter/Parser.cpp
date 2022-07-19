//
// Created by seeu on 2022/7/9.
//

#include "Parser.h"
#include <sstream>
#include <optional>
#include <stdexcept>

namespace GI {

    std::string ParserError::toString() {
        return message;
    }

    std::unique_ptr<Identifier> Parser::parseIdentifier() {
        return std::make_unique<Identifier>(Token{currentToken}, currentToken.literal);
    }

    void Parser::nextToken() {
        currentToken = peekToken;
        peekToken = lexer->nextToken();
    }

    std::unique_ptr<Program> Parser::parseProgram() {
        std::vector<std::unique_ptr<Statement>> statements{};
        nextToken(); // skip start

        while (currentToken.type != TokenType::_EOF) {
            auto stmt = parseStatement();
            if (stmt != nullptr) {
                statements.push_back(std::move(stmt));
            }
            nextToken();
        }

        return std::make_unique<Program>(std::move(statements));
    }

    std::unique_ptr<Statement> Parser::parseStatement() {
        nextToken();
        switch (currentToken.type) {
            case TokenType::LET:
                return parseLetStatement();
            case TokenType::RETURN:
                return parseReturnStatement();
            default:
                return parseExpressionStatement();
        }
    }

    std::unique_ptr<LetStatement> Parser::parseLetStatement() {
        auto token = currentToken;
        if (!expectPeekAndConsume(TokenType::IDENT)) {
            return nullptr;
        }
        auto name = std::make_unique<Identifier>(currentToken, currentToken.literal);
        if (!expectPeekAndConsume(TokenType::ASSIGN)) {
            return nullptr;
        }
        nextToken();
        auto value = parseExpression(Precedence::LOWEST);
        if (peekToken.type == TokenType::SEMICOLON) {
            nextToken();
        }
        return std::make_unique<LetStatement>(token, std::move(name), std::move(value));
    }

    bool Parser::expectPeekAndConsume(TokenType tokenType) {
        if (peekToken.type == tokenType) {
            nextToken();
            return true;
        } else {
            peekMatchError(tokenType);
            return false;
        }
    }

    void Parser::peekMatchError(TokenType tokenType) {
        std::stringstream ss{};
        ss << "expected next token to be " << tokenTypeMapping.map[tokenType] << ", ";
        ss << "got " << tokenTypeMapping.map[peekToken.type] << " instead";
        errors.push_back(std::make_unique<ParserError>(peekToken, ss.str()));
    }

    std::unique_ptr<Expression> Parser::parseExpression(Precedence precedence) {
        std::unique_ptr<Expression> expression;
        switch (currentToken.type) {
            case TokenType::IDENT:
                expression = std::make_unique<Identifier>(currentToken, currentToken.literal);
                break;
            case TokenType::INT:
                expression = parseIntegerExpression();
                break;
            case TokenType::BANG: // falls through
            case TokenType::MINUS:
                expression = parsePrefixExpression();
                break;
            case TokenType::TRUE:
            case TokenType::FALSE:
                expression = parseBoolExpression();
                break;
            case TokenType::LPAREN:
                expression = parseGroupedExpression();
                break;
            case TokenType::IF:
                expression = parseIfExpression();
                break;
            case TokenType::FUNCTION:
                expression = parseFunctionExpression();
                break;
            default:
                // skip
                break;

        }

        while (peekToken.type != TokenType::SEMICOLON && precedence < peekPrecedence()) {
            switch (peekToken.type) {
                case TokenType::PLUS:
                case TokenType::MINUS:
                case TokenType::SLASH:
                case TokenType::ASTERISK:
                case TokenType::EQ:
                case TokenType::NOT_EQ:
                case TokenType::LT:
                case TokenType::GT:
                    nextToken(); // skip infix token
                    expression = parseInfixExpression(std::move(expression));
                    break;
                case TokenType::LPAREN:
                    expression = parseCallExpression(std::move(expression));
                    break;
                default:
                    return expression;
            }

        }
        return expression;
    }

    std::unique_ptr<IntegerExpression> Parser::parseIntegerExpression() {
        try {
            auto value = std::stoi(currentToken.literal);
            return std::make_unique<IntegerExpression>(currentToken, value);
        }
        catch (std::exception const &ex) {
            errors.push_back(std::make_unique<ParserError>(currentToken,
                                                           "could not parse value " + currentToken.literal +
                                                           " as integer"));
            return nullptr;
        }
    }

    std::unique_ptr<PrefixExpression> Parser::parsePrefixExpression() {
        auto token = currentToken;
        nextToken();

        auto expression = parseExpression(Precedence::PREFIX);
        return std::make_unique<PrefixExpression>(token, token.literal, std::move(expression));
    }

    std::unique_ptr<BoolExpression> Parser::parseBoolExpression() {
        return std::make_unique<BoolExpression>(currentToken, currentToken.literal == "true");
    }

    std::unique_ptr<Expression> Parser::parseGroupedExpression() {
        nextToken();
        auto expr = parseExpression(Precedence::LOWEST);
        if (peekToken.type == TokenType::RPAREN) {
            return expr;
        }
        return nullptr;
    }

    std::unique_ptr<FunctionExpression> Parser::parseFunctionExpression() {
        auto token = currentToken;
        if (expectPeekAndConsume(TokenType::LPAREN)) {
            return nullptr;
        }

        auto parameters = parseFunctionParameters();
        if (!expectPeekAndConsume(TokenType::LBRACE)) {
            return nullptr;
        }

        auto body = parseBlockStatement();

        return std::make_unique<FunctionExpression>(token, std::move(parameters), std::move(body));
    }

    std::unique_ptr<BlockStatement> Parser::parseBlockStatement() {
        auto token = currentToken;
        std::vector<std::unique_ptr<Statement>> statements;
        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::_EOF) {
            auto stmt = parseStatement();
            if (stmt != nullptr) {
                statements.push_back(std::move(stmt));
            }
        }
        return std::make_unique<BlockStatement>(token, std::move(statements));
    }

    std::vector<std::unique_ptr<Identifier>> Parser::parseFunctionParameters() {
        std::vector<std::unique_ptr<Identifier>> parameters;
        if (peekToken.type == TokenType::RPAREN) {
            nextToken();
            return parameters;
        }
        nextToken();

        parameters.push_back(std::make_unique<Identifier>(currentToken, currentToken.literal));

        while (peekToken.type == TokenType::COMMA) {
            nextToken();
            nextToken();

            parameters.push_back(std::make_unique<Identifier>(currentToken, currentToken.literal));
        }
        if (!expectPeekAndConsume(TokenType::RPAREN)) {
            return {};
        }
        return parameters;
    }

    std::unique_ptr<IfExpression> Parser::parseIfExpression() {
        auto token = currentToken;
        if (!expectPeekAndConsume(TokenType::LPAREN)) {
            return nullptr;
        }
        nextToken();

        auto condition = parseExpression(Precedence::LOWEST);

        if (!expectPeekAndConsume(TokenType::RPAREN)) {
            return nullptr;
        }
        if (!expectPeekAndConsume(TokenType::LBRACE)) {
            return nullptr;
        }

        auto consequence = parseBlockStatement();
        std::unique_ptr<BlockStatement> alternative = nullptr;
        if (expectPeekAndConsume(TokenType::ELSE)) {
            nextToken();

            if (!expectPeekAndConsume(TokenType::LBRACE)) {
                return nullptr;
            }
            alternative = parseBlockStatement();

        }
        return std::make_unique<IfExpression>(token, std::move(condition), std::move(consequence),
                                              std::move(alternative));
    }

    std::unique_ptr<ReturnStatement> Parser::parseReturnStatement() {
        auto token = currentToken;
        nextToken();

        auto returnValue = parseExpression(Precedence::LOWEST);

        if (expectPeekAndConsume(TokenType::SEMICOLON)) {
            nextToken();
        }
        return std::make_unique<ReturnStatement>(token, std::move(returnValue));
    }

    std::unique_ptr<ExpressionStatement> Parser::parseExpressionStatement() {
        auto token = currentToken;
        auto expr = parseExpression(Precedence::LOWEST);
        if (peekToken.type == TokenType::SEMICOLON) {
            nextToken();
        }
        return std::make_unique<ExpressionStatement>(token, std::move(expr));
    }

    Precedence Parser::peekPrecedence() {
        if (precedences.contains(peekToken.type)) {
            return precedences[peekToken.type];
        }
        return Precedence::LOWEST;
    }

    std::unique_ptr<InfixExpression> Parser::parseInfixExpression(std::unique_ptr<Expression> left) {
        auto token = currentToken;
        auto precedence = precedences[currentToken.type];
        nextToken();
        auto right = parseExpression(precedence);
        return std::make_unique<InfixExpression>(token, std::move(left), std::move(right), token.literal);
    }

    std::unique_ptr<CallExpression> Parser::parseCallExpression(std::unique_ptr<Expression> left) {
        auto token = currentToken;
        auto arguments = parseCallArguments();
        return std::make_unique<CallExpression>(token, std::move(left), std::move(arguments));
    }

    std::vector<std::unique_ptr<Expression>> Parser::parseCallArguments() {
        std::vector<std::unique_ptr<Expression>> args;

        if (currentToken.type == TokenType::RPAREN) {
            nextToken();
            return args;
        }
        nextToken();
        args.push_back(parseExpression(Precedence::LOWEST));

        while (peekToken.type == TokenType::COMMA) {
            nextToken();
            nextToken();
            args.push_back(parseExpression(Precedence::LOWEST));
        }

        if (!expectPeekAndConsume(TokenType::RPAREN)) {
            return {};
        }
        return args;
    }
}
