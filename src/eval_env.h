//
// Created by timetraveler314 on 5/3/24.
//

#ifndef MINI_LISP_EVAL_ENV_H
#define MINI_LISP_EVAL_ENV_H

#include "value.h"

class EvalEnv {
    std::unordered_map<std::string, ValuePtr> symbolTable;

public:
    EvalEnv();

    void addSymbol(const std::string& symbol, ValuePtr value);

    ValuePtr eval(ValuePtr expr);
    ValuePtr apply(ValuePtr proc, std::vector<ValuePtr> args);

    // std::vector<ValuePtr> evalList(ValuePtr expr);

    std::optional<ValuePtr> lookup(const std::string& symbol) const;
};

#endif //MINI_LISP_EVAL_ENV_H
