//
// Created by timetraveler314 on 5/4/24.
//

#include <numeric>
#include <iostream>
#include <cmath>
#include <cfenv>
#include "builtins.h"
#include "eval_env.h"

const std::unordered_map<std::string, ValuePtr> Builtins::builtinMap = {
    // Core Library
    {"apply", std::make_shared<BuiltinProcValue>(_apply)},
    {"display", std::make_shared<BuiltinProcValue>(_display)},
    {"displayln", std::make_shared<BuiltinProcValue>(_displayln)},
    {"error", std::make_shared<BuiltinProcValue>(_error)},
    {"eval", std::make_shared<BuiltinProcValue>(_eval)},
    {"exit", std::make_shared<BuiltinProcValue>(_exit)},
    {"newline", std::make_shared<BuiltinProcValue>(_newline)},
    {"print", std::make_shared<BuiltinProcValue>(_print)},

    // TypeCheckers
    {"atom?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<AtomicValue>();
    }))},
    {"boolean?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<BooleanValue>();
    }))},
    {"integer?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isNumericInteger();
    }))},
    {"list?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->isList();
    }))},
    {"number?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<NumericValue>();
    }))},
    {"pair?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<PairValue>();
    }))},
    {"null?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<NilValue>();
    }))},
    {"string?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<StringValue>();
    }))},
    {"symbol?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<SymbolValue>();
    }))},
    {"procedure?", std::make_shared<BuiltinProcValue>(typeCheckerT([](const ValuePtr& v) {
            return v->is<ProcedureValue>();
    }))},

    // List functions
    {"append", std::make_shared<BuiltinProcValue>(_append)},
    {"car", std::make_shared<BuiltinProcValue>(_car)},
    {"cdr", std::make_shared<BuiltinProcValue>(_cdr)},
    {"cons", std::make_shared<BuiltinProcValue>(_cons)},
    {"length", std::make_shared<BuiltinProcValue>(_length)},
    {"list", std::make_shared<BuiltinProcValue>(_list)},
    {"map", std::make_shared<BuiltinProcValue>(_map)},
    {"filter", std::make_shared<BuiltinProcValue>(_filter)},
    {"reduce", std::make_shared<BuiltinProcValue>(_reduce)},

    {"+", std::make_shared<BuiltinProcValue>(_add)},
    {"-", std::make_shared<BuiltinProcValue>(_sub)},
    {"*", std::make_shared<BuiltinProcValue>(_mul)},
    {"/", std::make_shared<BuiltinProcValue>(_div)},
    {"abs", std::make_shared<BuiltinProcValue>(_abs)},
    {"expt", std::make_shared<BuiltinProcValue>(_expt)},
    {"quotient", std::make_shared<BuiltinProcValue>(_quotient)},
    // {"remainder", std::make_shared<BuiltinProcValue>(_remainder)},
    // {"modulo", std::make_shared<BuiltinProcValue>(_modulo)},

    {"eq?", std::make_shared<BuiltinProcValue>(_eq)},
    {"equal?", std::make_shared<BuiltinProcValue>(_equal)},
    {"not", std::make_shared<BuiltinProcValue>(_not)},
    {"=", std::make_shared<BuiltinProcValue>(_eq_num)},
    {"<", std::make_shared<BuiltinProcValue>(_lt)},
    {">", std::make_shared<BuiltinProcValue>(_gt)},
    {"<=", std::make_shared<BuiltinProcValue>(_le)},
    {">=", std::make_shared<BuiltinProcValue>(_ge)},
    {"even?", std::make_shared<BuiltinProcValue>(_is_even)},
    {"odd?", std::make_shared<BuiltinProcValue>(_is_odd)},
    {"zero?", std::make_shared<BuiltinProcValue>(_is_zero)},
};

ValuePtr Builtins::_apply(const std::vector<ValuePtr> &params, EvalEnv &env) {
    auto [proc, args] = Utils::resolveParams("apply", params, Utils::isProcedure, Utils::isList);
    return env.apply(proc, args);
}

ValuePtr Builtins::_display(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("display", 1, params);
    if (auto str = params[0]->as<StringValue>()) {
        std::cout << *str;
    } else {
        std::cout << params[0]->toString();
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_displayln(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("displayln", 1, params);
    if (auto str = params[0]->as<StringValue>()) {
        std::cout << *str << std::endl;
    } else {
        std::cout << params[0]->toString() << std::endl;
    }
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_error(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("error", 1, params);
    if (auto str = params[0]->as<StringValue>()) {
        throw LispError(*str);
    } else {
        throw LispError(params[0]->toString());
    }
}

ValuePtr Builtins::_eval(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("eval", 1, params);
    return env.eval(params[0]);
}

ValuePtr Builtins::_exit(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.empty()) std::exit(0);
    auto [exitCode] = Utils::resolveParams("exit", params, Utils::isInteger);
    std::exit(exitCode);
}

ValuePtr Builtins::_newline(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::requireParams("newline", params);
    std::cout << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_print(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("print", 1, params);
    std::cout << params[0]->toString() << std::endl;
    return std::make_shared<NilValue>();
}

ValuePtr Builtins::_append(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto lists = Utils::resolveAllParams("append", params, Utils::isList);
    std::vector<ValuePtr> result;
    for (const auto& list : lists) {
        std::copy(list.begin(), list.end(), std::back_inserter(result));
    }
    return Value::fromVector(result);
}

ValuePtr Builtins::_car(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [pair] = Utils::resolveParams("car", params, Utils::isPair);
    return pair->getCar();
}

ValuePtr Builtins::_cdr(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [pair] = Utils::resolveParams("cdr", params, Utils::isPair);
    return pair->getCdr();
}

ValuePtr Builtins::_cons(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("cons", 2, params);
    return std::make_shared<PairValue>(params[0], params[1]);
}

ValuePtr Builtins::_length(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [list] = Utils::resolveParams("length", params, Utils::isList);
    return std::make_shared<NumericValue>(list.size());
}

ValuePtr Builtins::_list(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return Value::fromVector(params);
}

ValuePtr Builtins::_map(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [proc, list] = Utils::resolveParams("map", params, Utils::isProcedure, Utils::isList);
    std::vector<ValuePtr> result;
    for (const auto& i : list) {
        result.push_back(env.apply(proc, {i}));
    }
    return Value::fromVector(result);
}

ValuePtr Builtins::_filter(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [proc, list] = Utils::resolveParams("filter", params, Utils::isProcedure, Utils::isList);
    std::vector<ValuePtr> result;
    for (const auto& i : list) {
        auto res = env.apply(proc, {i});
        if (!res->is<BooleanValue>() || *res->as<BooleanValue>()) {
            result.push_back(i);
        }
    }
    return Value::fromVector(result);
}

ValuePtr Builtins::_reduce(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [proc, list] = Utils::resolveParams("reduce", params, Utils::isProcedure, Utils::isNonEmptyList);
    auto length = list.size();
    ValuePtr result = list[0];
    for (size_t i = 1; i < length; ++i) {
        result = env.apply(proc, {result, list[i]});
    }
    return result;
}

// (+ n1 n2 ... nk)
ValuePtr Builtins::_add(const std::vector<ValuePtr>& params, EvalEnv& env) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->is<NumericValue>()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += *(i->as<NumericValue>());
    }
    return std::make_shared<NumericValue>(result);
}

ValuePtr Builtins::_sub(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("-", 1, 2, params);
    if (params.size() == 1) {
        if (!params[0]->is<NumericValue>()) {
            throw LispError("-: Invalid argument.");
        }
        return std::make_shared<NumericValue>(-*(params[0]->as<NumericValue>()));
    } else {
        if (!params[0]->is<NumericValue>() || !params[1]->is<NumericValue>()) {
            throw LispError("-: Invalid argument.");
        }
        return std::make_shared<NumericValue>(*params[0]->as<NumericValue>() - *params[1]->as<NumericValue>());
    }
}

ValuePtr Builtins::_mul(const std::vector<ValuePtr>& params, EvalEnv& env) {
    return std::accumulate(params.begin(), params.end(), std::make_shared<NumericValue>(1.0), [](const ValuePtr& a, const ValuePtr& b) {
        if (!a->is<NumericValue>() || !b->is<NumericValue>()) {
            throw LispError("*: Invalid argument.");
        }
        return std::make_shared<NumericValue>(*a->as<NumericValue>() * *b->as<NumericValue>());
    });
}

ValuePtr Builtins::_div(const std::vector<ValuePtr>& params, EvalEnv& env) {
    if (params.size() == 1) {
        auto [y] = Utils::resolveParams("/", params, Utils::isNumeric);
        if (y == 0.0) throw LispError("/: Division by zero.");
        return std::make_shared<NumericValue>(1 / y);
    } else {
        auto [x, y] = Utils::resolveParams("/", params, Utils::isNumeric, Utils::isNumeric);
        if (y == 0.0) throw LispError("/: Division by zero.");
        return std::make_shared<NumericValue>(x / y);
    }
}

ValuePtr Builtins::_abs(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x] = Utils::resolveParams("abs", params, Utils::isNumeric);
    return std::make_shared<NumericValue>(std::abs(x));
}

ValuePtr Builtins::_expt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams("expt", params, Utils::isNumeric, Utils::isNumeric);

    std::feclearexcept(FE_ALL_EXCEPT);
    auto result = std::make_shared<NumericValue>(std::pow(x, y));
    if (std::fetestexcept(FE_INVALID)) throw LispError("expt: base is finite and negative and exp is finite and non-integer.");
    if (std::fetestexcept(FE_DIVBYZERO)) throw LispError("expt: base is zero and exp is negative.");
    return result;
}

ValuePtr Builtins::_quotient(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams("quotient", params, Utils::isNumeric, Utils::isNumeric);
    if (y == 0.0) throw LispError("quotient: Division by zero.");
    return std::make_shared<NumericValue>(std::trunc(x / y)); // TODO: Check
}

// Comparison functions

bool Builtins::_builtin_equal(const ValuePtr& x, const ValuePtr& y) {
    if (x->is<BooleanValue>()) {
        return y->is<BooleanValue>() && *x->as<BooleanValue>() == *y->as<BooleanValue>();
    } else if (x->is<NumericValue>()) {
        return y->is<NumericValue>() && *x->as<NumericValue>() == *y->as<NumericValue>();
    } else if (x->is<StringValue>()) {
        return y->is<StringValue>() && *x->as<StringValue>() == *y->as<StringValue>();
    } else if (x->is<SymbolValue>()) {
        return y->is<SymbolValue>() && *(x->asSymbol()) == *(y->asSymbol());
    } else if (x->is<NilValue>()) {
        return y->is<NilValue>();
    } else if (x->is<PairValue>()) {
        auto xPair = std::dynamic_pointer_cast<PairValue>(x);
        if (y->is<PairValue>()) {
            auto yPair = std::dynamic_pointer_cast<PairValue>(y);
            return _builtin_equal(xPair->getCar(), yPair->getCar()) && _builtin_equal(xPair->getCdr(), yPair->getCdr());
        } else {
            return false;
        }
    } else if (x->is<ProcedureValue>()) {
        if (y->is<ProcedureValue>()) {
            throw LispError("eq?: Comparing procedures is undefined.");
        } else {
            return false;
        }
    } else {
        return false;
    }
}

ValuePtr Builtins::_equal(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("eq?", 2, params);
    return std::make_shared<BooleanValue>(_builtin_equal(params[0], params[1]));
}

ValuePtr Builtins::_eq(const std::vector<ValuePtr>& params, EvalEnv& env) {
    Utils::checkParams("equal?", 2, params);
    auto x = params[0];
    auto y = params[1];
    bool result;
    if (x->is<BooleanValue>() || x->is<NumericValue>() || x->is<ProcedureValue>() || x->is<SymbolValue>() || x->is<NilValue>()) {
        result = _builtin_equal(x, y);
    } else if (x->is<StringValue>()) {
        result = x == y;
    } else if (x->is<PairValue>()) {
        result = x == y;
    } else {
        result = false;
    }

    return std::make_shared<BooleanValue>(result);
}

ValuePtr Builtins::_not(const std::vector<ValuePtr> &params, EvalEnv &env) {
    Utils::checkParams("not", 1, params);
    if (!params[0]->is<BooleanValue>() || *params[0]->as<BooleanValue>()) {
        return std::make_shared<BooleanValue>(false);
    } else {
        return std::make_shared<BooleanValue>(true);
    }
}

ValuePtr Builtins::_eq_num(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams("=", params, Utils::isNumeric, Utils::isNumeric);
    return std::make_shared<BooleanValue>(x == y);
}

ValuePtr Builtins::_lt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams("<", params, Utils::isNumeric, Utils::isNumeric);
    return std::make_shared<BooleanValue>(x < y);
}

ValuePtr Builtins::_gt(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams(">", params, Utils::isNumeric, Utils::isNumeric);
    return std::make_shared<BooleanValue>(x > y);
}

ValuePtr Builtins::_le(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams("<=", params, Utils::isNumeric, Utils::isNumeric);
    return std::make_shared<BooleanValue>(x <= y);
}

ValuePtr Builtins::_ge(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x, y] = Utils::resolveParams(">=", params, Utils::isNumeric, Utils::isNumeric);
    return std::make_shared<BooleanValue>(x >= y);
}

ValuePtr Builtins::_is_even(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x] = Utils::resolveParams("even?", params, Utils::isNumeric);
    return std::make_shared<BooleanValue>(std::fmod(x, 2) == 0);
}

ValuePtr Builtins::_is_odd(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x] = Utils::resolveParams("odd?", params, Utils::isNumeric);
    return std::make_shared<BooleanValue>(std::fmod(x, 2) != 0);
}

ValuePtr Builtins::_is_zero(const std::vector<ValuePtr>& params, EvalEnv& env) {
    auto [x] = Utils::resolveParams("zero?", params, Utils::isNumeric);
    return std::make_shared<BooleanValue>(x == 0);
}