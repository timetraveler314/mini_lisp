//
// Created by timetraveler314 on 5/12/24.
//
#include <ranges>
#include "forms.h"
#include "error.h"
#include "utils/utils.h"

namespace SpecialForms {
    const std::unordered_map<std::string, SpecialFormType> SPECIAL_FORMS {
        {"define", _define},
        {"quote", _quote},
        {"if", _if},
        {"cond", _cond},
        {"let", _let},
        {"begin", _begin},
        {"quasiquote", _quasiquote},
        {"unquote", _unquote},
        {"and", _and},
        {"or", _or},
        {"lambda", _lambda},
        {"λ", _lambda},
    };

    ValuePtr _define(const std::vector<ValuePtr> &params, EvalEnv &env) {
        if (params.size() == 0) throw LispError("define: expected at least 2 arguments.");
        if (auto symbol = std::dynamic_pointer_cast<SymbolValue>(params[0])) {
            Utils::checkParams("define", 2, params);
            env.defineBinding(symbol->getValue(), env.eval(params[1]));
        } else if (auto pair = std::dynamic_pointer_cast<PairValue>(params[0])) {
            if (pair->getCar()->is<SymbolValue>()) {
                if (params.size() < 2) {
                    throw LispError("define: expected at least 2 arguments.");
                }
                auto symbol = pair->getCar()->asSymbol();

                auto viewLambdaParams = pair->getCdr()->toVector()
                                        | std::views::transform([](const ValuePtr& v) { return v->asSymbol(); })
                                        | std::views::transform([](const std::optional<std::string>& s) {
                    if (!s) {
                        throw LispError("lambda: Arguments must be symbols.");
                    }
                    return *s;
                });
                std::vector<std::string> lambdaParams(viewLambdaParams.begin(), viewLambdaParams.end());

                auto lambdaBody = std::vector(params.begin() + 1, params.end());
                env.defineBinding(*symbol, std::make_shared<LambdaValue>(env.shared_from_this(), std::move(lambdaParams), std::move(lambdaBody)));
                return std::make_shared<NilValue>();
            } else {
                throw LispError("define: Invalid expression.");
            }
        } else{
            throw LispError("define: Invalid expression.");
        }

        return std::make_shared<NilValue>();
    }

    ValuePtr _quote(const std::vector<ValuePtr> &params, EvalEnv &env) {
        Utils::checkParams("quote", 1, params);
        return params[0];
    }

    ValuePtr _quasiquote_impl(const ValuePtr& value, EvalEnv& env) {
        if (auto pair = std::dynamic_pointer_cast<PairValue>(value)) {
            if (pair->getCar()->is<SymbolValue>() && *pair->getCar()->asSymbol() == "unquote") {
                Utils::checkParams("unquote", 1, pair->getCdr()->toVector());
                return env.eval(pair->getCdr()->toVector()[0]);
            } else {
                return std::make_shared<PairValue>(_quasiquote_impl(pair->getCar(), env),
                                                   _quasiquote_impl(pair->getCdr(), env));
            }
        } else {
            return value;
        }
    }

    ValuePtr _quasiquote(const std::vector<ValuePtr> &params, EvalEnv &env) {
        Utils::checkParams("quasiquote", 1, params);
        return _quasiquote_impl(params[0], env);
    }

    ValuePtr _unquote(const std::vector<ValuePtr> &params, EvalEnv &env) {
        throw LispError("unquote: Invalid context.");
    }

    ValuePtr _if(const std::vector<ValuePtr> &params, EvalEnv &env) {
        Utils::checkParams("if", 2, 3, params);
        auto condition = env.eval(params[0]);
        if (Utils::isFalse(condition)) {
            if (params.size() == 3) return env.eval(params[2]);
            else return std::make_shared<NilValue>(); // Undefined behavior, return ()
        } else {
            return env.eval(params[1]);
        }
    }

    ValuePtr _and(const std::vector<ValuePtr> &params, EvalEnv &env) {
        if (params.empty()) {
            return std::make_shared<BooleanValue>(true);
        }
        ValuePtr evalResult;
        for (const auto& param : params) {
            evalResult = env.eval(param);
            if (Utils::isFalse(evalResult)) {
                return std::make_shared<BooleanValue>(false);
            }
        }
        return evalResult;
    }

    ValuePtr _or(const std::vector<ValuePtr> &params, EvalEnv &env) {
        if (params.empty()) {
            return std::make_shared<BooleanValue>(false);
        }

        ValuePtr evalResult;
        for (const auto& param : params) {
            evalResult = env.eval(param);
            if (!Utils::isFalse(evalResult)) {
                break;
            }
        }
        return evalResult;
    }

    ValuePtr _cond(const std::vector<ValuePtr> &params, EvalEnv &env) {
        for (auto it = params.begin(); it != params.end(); it++) {
            const auto& param = *it;
            if (auto pair = std::dynamic_pointer_cast<PairValue>(param)) {
                if (pair->getCar()->is<SymbolValue>() && *pair->getCar()->asSymbol() == "else") {
                    if (pair->getCdr()->is<NilValue>()) {
                        throw LispError("cond: `else` must be followed by an expression.");
                    }
                    if (it + 1 != params.end()) throw LispError("cond: `else` must be the last clause.");
                    return env.evalList(pair->getCdr()).back();
                }
                ValuePtr result = env.eval(pair->getCar());
                if (!Utils::isFalse(result)) {
                    if (pair->getCdr()->is<NilValue>()) {
                        return result;
                    }
                    return env.evalList(pair->getCdr()).back();
                }
            } else {
                throw LispError("cond: Invalid expression.");
            }
        }
        return std::make_shared<NilValue>();
    }

    ValuePtr _begin(const std::vector<ValuePtr> &params, EvalEnv &env) {
        if (params.empty()) {
            return std::make_shared<NilValue>();
        }
        ValuePtr result;
        for (const auto& expr : params) {
            result = env.eval(expr);
        }
        return result;
    }

    ValuePtr _let(const std::vector<ValuePtr> &params, EvalEnv &env) {
        if (params.size() < 2) throw LispError("let: expected at least 2 arguments.");
        if (!params[0]->isList()) throw LispError("let: Invalid expression.");
        auto list = params[0]->toVector();
        std::vector<std::string> letParams;
        std::vector<ValuePtr> letValues;
        for (const auto& bind : list) {
            if (!bind->is<PairValue>()) throw LispError("let: Invalid expression.");
            auto bindPair = std::dynamic_pointer_cast<PairValue>(bind);
            if (auto symbol = bindPair->getCar()->asSymbol()) {
                try {
                    auto cdrVector = bindPair->getCdr()->toVector();
                    if (cdrVector.size() != 1) throw LispError("let: Invalid expression.");
                    auto value = env.eval(cdrVector[0]);
                    letParams.push_back(*symbol);
                    letValues.push_back(value);
                } catch (const LispError& e) {
                    throw LispError("let: Invalid expression.");
                }
            } else throw LispError("let: Expected symbol in the binding list.");
        }
        auto childEnv = env.createChild(letParams, letValues);

        ValuePtr result;
        for (size_t i = 1; i < params.size(); i++) {
            result = childEnv->eval(params[i]);
        }
        return result;
    }

    ValuePtr _lambda(const std::vector<ValuePtr> &params, EvalEnv &env) {
        std::vector<std::string> lambdaParams;
        if (params[0]->is<SymbolValue>()) {
            lambdaParams.push_back(*params[0]->asSymbol());
        } else {
            auto viewLambdaParams = params[0]->toVector()
                                    | std::views::transform([](const ValuePtr& v) { return v->asSymbol(); })
                                    | std::views::transform([](const std::optional<std::string>& s) {
                if (!s) {
                    throw LispError("lambda: Arguments must be symbols.");
                }
                return *s;
            });
            lambdaParams = {viewLambdaParams.begin(), viewLambdaParams.end()};
        }
        auto lambdaBody = std::vector(params.begin() + 1, params.end());
        return std::make_shared<LambdaValue>(env.shared_from_this(), std::move(lambdaParams), std::move(lambdaBody));
    }

    ValuePtr _delay(const std::vector<ValuePtr> &params, EvalEnv &env) {
        Utils::checkParams("delay", 1, params);
        return std::make_shared<LambdaValue>(env.shared_from_this(), std::vector<std::string>(), params);
    }
}