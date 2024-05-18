//
// Created by timetraveler314 on 5/4/24.
//

#ifndef MINI_LISP_BUILTINS_H
#define MINI_LISP_BUILTINS_H

// Built-in procedures

#include "value.h"
#include "error.h"
#include "utils/utils.h"

namespace Builtins {
    extern const std::unordered_map<std::string, ValuePtr> builtinMap;

    // 7.1 Core Library
    ValuePtr _apply(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _display(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _displayln(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _error(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _eval(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _exit(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _newline(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _print(const std::vector<ValuePtr>& params, EvalEnv& env);

    // 7.2 Type Predicates
    template<typename F>
    inline BuiltinFuncType typeCheckerT(F f) {
        return [f](const std::vector<ValuePtr>& params, EvalEnv& env) {
            Utils::checkParams("builtin-type-checker", 1, 1, params);
            return std::make_shared<BooleanValue>(f(params[0]));
        };
    }

    // 7.3 List functions
    ValuePtr _append(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _car(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _cdr(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _cons(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _length(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _list(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _map(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _filter(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _reduce(const std::vector<ValuePtr>& params, EvalEnv& env);

    // 7.4 Arithmetic Functions
    ValuePtr _add(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _sub(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _mul(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _div(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _abs(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _expt(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _quotient(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _remainder(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _modulo(const std::vector<ValuePtr>& params, EvalEnv& env);

    // 7.5 Comparison Library
    ValuePtr _eq(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _equal(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _not(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _eq_num(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _lt(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _gt(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _le(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _ge(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _is_even(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _is_odd(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _is_zero(const std::vector<ValuePtr>& params, EvalEnv& env);

    bool _builtin_equal(const ValuePtr &x, const ValuePtr &y);
}

#endif //MINI_LISP_BUILTINS_H
