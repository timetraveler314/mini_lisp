//
// Created by timetraveler314 on 5/4/24.
//

#ifndef MINI_LISP_BUILTINS_H
#define MINI_LISP_BUILTINS_H

// Built-in procedures

#include "value.h"
#include "error.h"
#include "utils.h"

namespace Builtins {
    extern const std::unordered_map<std::string, ValuePtr> builtinMap;

    ValuePtr _display(const std::vector<ValuePtr>& params);
    ValuePtr _displayln(const std::vector<ValuePtr>& params);
    ValuePtr _error(const std::vector<ValuePtr>& params);
    ValuePtr _exit(const std::vector<ValuePtr> &params);
    ValuePtr _newline(const std::vector<ValuePtr>& params);
    ValuePtr _print(const std::vector<ValuePtr>& params);

    template<typename F>
    inline BuiltinFuncType typeCheckerT(F f) {
        return [f](const std::vector<ValuePtr>& params) {
            Utils::checkParams(params, 1, 1, "builtin-type-checker");
            return std::make_shared<BooleanValue>(f(params[0]));
        };
    }

    // List functions
    ValuePtr _car(const std::vector<ValuePtr>& params);
    ValuePtr _cdr(const std::vector<ValuePtr>& params);
    ValuePtr _cons(const std::vector<ValuePtr>& params);
    ValuePtr _length(const std::vector<ValuePtr>& params);
    ValuePtr _list(const std::vector<ValuePtr>& params);
    ValuePtr _append(const std::vector<ValuePtr>& params);

    ValuePtr _add(const std::vector<ValuePtr>& params);
    ValuePtr _sub(const std::vector<ValuePtr>& params);
    ValuePtr _mul(const std::vector<ValuePtr>& params);
    ValuePtr _div(const std::vector<ValuePtr>& params);
    ValuePtr _abs(const std::vector<ValuePtr>& params);
    ValuePtr _expt(const std::vector<ValuePtr>& params);
    ValuePtr _quotient(const std::vector<ValuePtr>& params);
    ValuePtr _remainder(const std::vector<ValuePtr>& params);
    ValuePtr _modulo(const std::vector<ValuePtr>& params);

    ValuePtr _eq(const std::vector<ValuePtr>& params);
    ValuePtr _equal(const std::vector<ValuePtr>& params);
    ValuePtr _not(const std::vector<ValuePtr>& params);
    ValuePtr _eq_num(const std::vector<ValuePtr>& params);
    ValuePtr _lt(const std::vector<ValuePtr>& params);
    ValuePtr _gt(const std::vector<ValuePtr>& params);
    ValuePtr _le(const std::vector<ValuePtr>& params);
    ValuePtr _ge(const std::vector<ValuePtr>& params);
    ValuePtr _is_even(const std::vector<ValuePtr>& params);
    ValuePtr _is_odd(const std::vector<ValuePtr>& params);
    ValuePtr _is_zero(const std::vector<ValuePtr>& params);
}

#endif //MINI_LISP_BUILTINS_H
