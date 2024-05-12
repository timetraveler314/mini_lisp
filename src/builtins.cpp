//
// Created by timetraveler314 on 5/4/24.
//

#include <numeric>
#include <iostream>
#include <cmath>
#include <cfenv>
#include "builtins.h"

const std::unordered_map<std::string, ValuePtr> Builtins::builtinMap = {
    {"display", std::make_shared<BuiltinProcValue>(_display)},
    {"displayln", std::make_shared<BuiltinProcValue>(_displayln)},
    {"error", std::make_shared<BuiltinProcValue>(_error)},
    {"exit", std::make_shared<BuiltinProcValue>(_exit)},
    {"newline", std::make_shared<BuiltinProcValue>(_newline)},
    {"print", std::make_shared<BuiltinProcValue>(_print)},

    // TypeCheckers
    {"atom?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isAtomic();
    }))},
    {"boolean?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isBoolean();
    }))},
    {"integer?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isNumericInteger();
    }))},
    {"list?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isNonEmptyList() || v->isNil();
    }))},
    {"number?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isNumber();
    }))},
    {"pair?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isPair();
    }))},
    {"null?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isNil();
    }))},
    {"string?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isString();
    }))},
    {"symbol?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isSymbol();
    }))},
    {"procedure?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isBuiltinProc(); // TODO: Lambda
    }))},

    // List functions
    {"car", std::make_shared<BuiltinProcValue>(_car)},
    {"cdr", std::make_shared<BuiltinProcValue>(_cdr)},
    {"cons", std::make_shared<BuiltinProcValue>(_cons)},
    {"length", std::make_shared<BuiltinProcValue>(_length)},
    {"list", std::make_shared<BuiltinProcValue>(_list)},

    {"+", std::make_shared<BuiltinProcValue>(_add)},
    {"-", std::make_shared<BuiltinProcValue>(_sub)},
    {"*", std::make_shared<BuiltinProcValue>(_mul)},
    {"/", std::make_shared<BuiltinProcValue>(_div)},
    {"abs", std::make_shared<BuiltinProcValue>(_abs)},
    {"expt", std::make_shared<BuiltinProcValue>(_expt)},
    {"quotient", std::make_shared<BuiltinProcValue>(_quotient)},
    // {"remainder", std::make_shared<BuiltinProcValue>(_remainder)},
    // {"modulo", std::make_shared<BuiltinProcValue>(_modulo)},

    {"=", std::make_shared<BuiltinProcValue>(_eq_num)},
    {"<", std::make_shared<BuiltinProcValue>(_lt)},
    {">", std::make_shared<BuiltinProcValue>(_gt)},
    {"<=", std::make_shared<BuiltinProcValue>(_le)},
    {">=", std::make_shared<BuiltinProcValue>(_ge)},
    {"even?", std::make_shared<BuiltinProcValue>(_is_even)},
    {"odd?", std::make_shared<BuiltinProcValue>(_is_odd)},
    {"zero?", std::make_shared<BuiltinProcValue>(_is_zero)},
};

ValuePtr Builtins::_display(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "display");
    if (auto str = params[0]->asString()) {
        std::cout << *str;
    } else {
        std::cout << params[0]->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_displayln(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "displayln");
    if (auto str = params[0]->asString()) {
        std::cout << *str << std::endl;
    } else {
        std::cout << params[0]->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_error(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "error");
    if (auto str = params[0]->asString()) {
        throw LispError(*str);
    } else {
        throw LispError(params[0]->toString());
    }
}

ValuePtr Builtins::_exit(const std::vector<ValuePtr> &params) {
    if (params.empty()) std::exit(0);
    Utils::checkParams(params, 0, 1, "exit");
    if (params[0]->asInteger()) {
        std::exit(*params[0]->asInteger());
    } else {
        throw LispError("exit: Invalid argument.");
    }
}

ValuePtr Builtins::_newline(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 0, "newline");
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_print(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "print");
    std::cout << params[0]->toString() << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_car(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "car");
    if (params[0]->isPair()) {
        auto pair = std::dynamic_pointer_cast<PairValue>(params[0]);
        return pair->getCar();
    } else {
        throw LispError("car: Invalid argument.");
    }
}

ValuePtr Builtins::_cdr(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "cdr");
    if (params[0]->isPair()) {
        auto pair = std::dynamic_pointer_cast<PairValue>(params[0]);
        return pair->getCdr();
    } else {
        throw LispError("cdr: Invalid argument.");
    }
}

ValuePtr Builtins::_cons(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, "cons");
    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr Builtins::_length(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "length");
    try {
        auto list = params[0]->toVector();
        return std::make_shared<NumericValue>(list.size());
    } catch (const LispError& e) {
        throw LispError("length: Invalid argument.");
    }
    throw LispError("length: Invalid argument.");
}

ValuePtr Builtins::_list(const std::vector<ValuePtr> &params) {
    return Value::fromVector(params);
}

// (+ n1 n2 ... nk)
ValuePtr Builtins::_add(const std::vector<ValuePtr>& params) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += *(i->asNumber());
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr Builtins::_sub(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, 2, "-");
    if (params.size() == 1) {
        if (!params[0]->isNumber()) {
            throw LispError("-: Invalid argument.");
        }
        return std::make_shared<NumericValue>(-*(params[0]->asNumber()));
    } else {
        if (!params[0]->isNumber() || !params[1]->isNumber()) {
            throw LispError("-: Invalid argument.");
        }
        return std::make_shared<NumericValue>(*params[0]->asNumber() - *params[1]->asNumber());
    }
}

ValuePtr Builtins::_mul(const std::vector<ValuePtr> &params) {
    return std::accumulate(params.begin(), params.end(), std::make_shared<NumericValue>(1.0), [](const ValuePtr& a, const ValuePtr& b) {
        if (!a->isNumber() || !b->isNumber()) {
            throw LispError("*: Invalid argument.");
        }
        return std::make_shared<NumericValue>(*a->asNumber() * *b->asNumber());
    });
}

ValuePtr Builtins::_div(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, 2, "/");
    if (params.size() == 1) {
        if (!params[0]->isNumber()) {
            throw LispError("/: Invalid argument.");
        }
        if (*params[0]->asNumber() == 0.0) {
            throw LispError("/: Division by zero.");
        }
        return std::make_shared<NumericValue>(1 / *params[0]->asNumber());
    } else {
        if (!params[0]->isNumber() || !params[1]->isNumber()) {
            throw LispError("/: Invalid argument.");
        }
        if (*params[1]->asNumber() == 0.0) {
            throw LispError("/: Division by zero.");
        }
        return std::make_shared<NumericValue>(*params[0]->asNumber() / *params[1]->asNumber());
    }
}

ValuePtr Builtins::_abs(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "abs");
    if (!params[0]->isNumber()) {
        throw LispError("abs: Invalid argument.");
    }
    return std::make_shared<NumericValue>(std::abs(*params[0]->asNumber()));
}

ValuePtr Builtins::_expt(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, "expt");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("expt: Invalid argument.");
    }

    std::feclearexcept(FE_ALL_EXCEPT);
    auto result = std::make_shared<NumericValue>(std::pow(*params[0]->asNumber(), *params[1]->asNumber()));
    if (std::fetestexcept(FE_INVALID)) throw LispError("expt: base is finite and negative and exp is finite and non-integer.");
    if (std::fetestexcept(FE_DIVBYZERO)) throw LispError("expt: base is zero and exp is negative.");
    return result;
}

ValuePtr Builtins::_quotient(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, "quotient");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("quotient: Invalid argument.");
    }
    if (*params[1]->asNumber() == 0.0) {
        throw LispError("quotient: Division by zero.");
    }
    return std::make_shared<NumericValue>(std::trunc(*params[0]->asNumber() / *params[1]->asNumber()));
}

// Comparison functions

ValuePtr Builtins::_eq_num(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, "=");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("=: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(*params[0]->asNumber() == *params[1]->asNumber());
}

ValuePtr Builtins::_lt(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, "<");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("<: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(*params[0]->asNumber() < *params[1]->asNumber());
}

ValuePtr Builtins::_gt(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, ">");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError(">: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(*params[0]->asNumber() > *params[1]->asNumber());
}

ValuePtr Builtins::_le(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, "<=");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError("<=: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(*params[0]->asNumber() <= *params[1]->asNumber());
}

ValuePtr Builtins::_ge(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 2, ">=");
    if (!params[0]->isNumber() || !params[1]->isNumber()) {
        throw LispError(">=: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(*params[0]->asNumber() >= *params[1]->asNumber());
}

ValuePtr Builtins::_is_even(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "even?");
    if (!params[0]->isNumber()) {
        throw LispError("even?: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(std::fmod(*params[0]->asNumber(), 2) == 0);
}

ValuePtr Builtins::_is_odd(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "odd?");
    if (!params[0]->isNumber()) {
        throw LispError("odd?: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(std::fmod(*params[0]->asNumber(), 2) != 0);
}

ValuePtr Builtins::_is_zero(const std::vector<ValuePtr> &params) {
    Utils::checkParams(params, 1, "zero?");
    if (!params[0]->isNumber()) {
        throw LispError("zero?: Invalid argument.");
    }
    return std::make_shared<BooleanValue>(*params[0]->asNumber() == 0);
}