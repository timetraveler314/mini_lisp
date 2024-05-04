//
// Created by timetraveler314 on 5/3/24.
//

#include "parser.h"
#include "error.h"

TokenPtr Parser::safePop() {
    if (tokens.empty()) {
        throw EOFError("Unexpected EOF");
    }
    auto token = std::move(tokens.front());
    tokens.pop_front();
    return token;
}

TokenType Parser::safePeekTokenType() const {
    if (tokens.empty()) {
        throw EOFError("Unexpected EOF");
    }
    return tokens.front()->getType();
}

ValuePtr Parser::parse() {
    TokenPtr token = safePop();

    switch (token->getType()) {
        case TokenType::NUMERIC_LITERAL:
            return std::make_shared<NumericValue>(static_cast<NumericLiteralToken&>(*token).getValue());
        case TokenType::BOOLEAN_LITERAL:
            return std::make_shared<BooleanValue>(static_cast<BooleanLiteralToken&>(*token).getValue());
        case TokenType::STRING_LITERAL:
            return std::make_shared<StringValue>(static_cast<StringLiteralToken&>(*token).getValue());
        case TokenType::IDENTIFIER:
            return std::make_shared<SymbolValue>(static_cast<IdentifierToken&>(*token).getName());
        case TokenType::LEFT_PAREN:
            return parseTails();
        case TokenType::QUOTE:
            return Value::fromVector({std::make_shared<SymbolValue>("quote"), parse()});
        case TokenType::UNQUOTE:
            return Value::fromVector({std::make_shared<SymbolValue>("unquote"), parse()});
        case TokenType::QUASIQUOTE:
            return Value::fromVector({std::make_shared<SymbolValue>("quasiquote"), parse()});
        case TokenType::RIGHT_PAREN:
            throw SyntaxError("Unexcepted ')'.");
        case TokenType::DOT:
            throw SyntaxError("Unexpected '.'.");
        default:
            throw SyntaxError("Unexpected token.");
    }
}

ValuePtr Parser::parseTails() {
    if (safePeekTokenType() == TokenType::RIGHT_PAREN) {
        tokens.pop_front();
        return std::make_shared<NilValue>();
    }

    auto car = this->parse();
    if (safePeekTokenType() == TokenType::DOT) {
        tokens.pop_front();
        auto cdr = this->parse();
        if (safePeekTokenType() != TokenType::RIGHT_PAREN) throw SyntaxError("Expected ')' after cdr.");

        tokens.pop_front();
        return std::make_shared<PairValue>(car, cdr);
    } else {
        auto cdr = this->parseTails();
        return std::make_shared<PairValue>(car, cdr);
    }
}