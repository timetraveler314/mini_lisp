//
// Created by timetraveler314 on 5/3/24.
//

#include "parser.h"
#include "error.h"
#include "tokenizer.h"

Utils::Task<ValuePtr> Parser::parse() {
    auto token = co_await tokenizer.awaitNextToken();

    switch (token->getType()) {
        case TokenType::NUMERIC_LITERAL:
            co_return std::make_shared<NumericValue>(static_cast<NumericLiteralToken&>(*token).getValue());
        case TokenType::BOOLEAN_LITERAL:
            co_return std::make_shared<BooleanValue>(static_cast<BooleanLiteralToken&>(*token).getValue());
        case TokenType::STRING_LITERAL:
            co_return std::make_shared<StringValue>(static_cast<StringLiteralToken&>(*token).getValue());
        case TokenType::IDENTIFIER:
            co_return std::make_shared<SymbolValue>(static_cast<IdentifierToken&>(*token).getName());
        case TokenType::LEFT_PAREN: {
            auto result = co_await parseTails();
            if (result->is<PairValue>()) {
                std::dynamic_pointer_cast<PairValue>(result)->position = token->position;
            }
            co_return result;
        }
        case TokenType::QUOTE:
            co_return Value::fromVector({std::make_shared<SymbolValue>("quote"), co_await parse()});
        case TokenType::UNQUOTE:
            co_return Value::fromVector({std::make_shared<SymbolValue>("unquote"), co_await parse()});
        case TokenType::QUASIQUOTE:
            co_return Value::fromVector({std::make_shared<SymbolValue>("quasiquote"), co_await parse()});
        case TokenType::RIGHT_PAREN:
            throw SyntaxError("Unexpected ')'.");
        case TokenType::DOT:
            throw SyntaxError("Unexpected '.'.");
        default:
            throw SyntaxError("Unexpected token.");
    }
}

Utils::Task<ValuePtr> Parser::parseTails() {
    if (co_await tokenizer.awaitPeekNextToken() == TokenType::RIGHT_PAREN) {
        co_await tokenizer.awaitNextToken(); // consume the right paren
        co_return std::make_shared<NilValue>();
    }

    auto car = co_await this->parse();
    if (co_await tokenizer.awaitPeekNextToken() == TokenType::DOT) {
        co_await tokenizer.awaitNextToken(); // consume the dot
        auto cdr = co_await this->parse();
        if (co_await tokenizer.awaitPeekNextToken() != TokenType::RIGHT_PAREN) {
            throw SyntaxError("Expected ')' after cdr.");
        }
        co_await tokenizer.awaitNextToken(); // consume the right paren
        co_return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = co_await this->parseTails();
        co_return std::make_shared<PairValue>(car, cdr);
    }
} 