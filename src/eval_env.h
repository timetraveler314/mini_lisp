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

    ValuePtr eval(ValuePtr expr);
    std::optional<ValuePtr> lookup(const std::string& symbol) const;
};

#endif //MINI_LISP_EVAL_ENV_H