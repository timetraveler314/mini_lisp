//
// Created by timetraveler314 on 5/3/24.
//

#ifndef MINI_LISP_PARSER_H
#define MINI_LISP_PARSER_H

#include <deque>
#include "token.h"
#include "value.h"

class Parser {
    std::deque<TokenPtr> tokens;
public:
    Parser(std::deque<TokenPtr> tokens): tokens{std::move(tokens)} {}

    TokenPtr safePop();
    TokenType safePeekTokenType() const;

    ValuePtr parse();
    ValuePtr parseTails();
};

#endif //MINI_LISP_PARSER_H
