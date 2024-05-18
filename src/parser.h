//
// Created by timetraveler314 on 5/3/24.
//

#ifndef MINI_LISP_PARSER_H
#define MINI_LISP_PARSER_H

#include <deque>
#include "token.h"
#include "value.h"
#include "tokenizer.h"
#include "utils/utils.h"
#include "utils/task.h"

class Parser {
    Tokenizer& tokenizer;

public:
    explicit Parser(Tokenizer& tokenizer): tokenizer{tokenizer} {}

    Utils::Task<ValuePtr> parse();
    Utils::Task<ValuePtr> parseTails();
};

#endif //MINI_LISP_PARSER_H
