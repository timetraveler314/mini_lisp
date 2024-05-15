//
// Created by timetraveler314 on 5/12/24.
//
#include <ranges>
#include "forms.h"
#include "error.h"
#include "utils.h"

namespace SpecialForms {
    const std::unordered_map<std::string, SpecialFormType> SPECIAL_FORMS {
        {"define", _define},
        {"quote", _quote},
        {"if", _if},
        {"and", _and},
        {"or", _or},
        {"lambda", _lambda}
    };

    ValuePtr _define(const std::vector<ValuePtr> &params, EvalEnv &env) {
        if (auto symbol = std::dynamic_pointer_cast<SymbolValue>(params[0])) {
            Utils::checkParams(params, 2, "define");
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
        Utils::checkParams(params, 1, "quote");
        return params[0];
    }

    ValuePtr _if(const std::vector<ValuePtr> &params, EvalEnv &env) {
        Utils::checkParams(params, 3, "if");
        auto condition = env.eval(params[0])->as<BooleanValue>();
        if (condition && !(*condition)) { // Condition evaluates to #f
            return env.eval(params[2]);
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
            if (evalResult->is<BooleanValue>() && !*evalResult->as<BooleanValue>()) {
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
            if (!evalResult->is<BooleanValue>() || *evalResult->as<BooleanValue>()) { // evalResult is not #f
                break;
            }
        }
        return evalResult;
    }

    ValuePtr _lambda(const std::vector<ValuePtr> &params, EvalEnv &env) {
        auto viewLambdaParams = params[0]->toVector()
                | std::views::transform([](const ValuePtr& v) { return v->asSymbol(); })
                | std::views::transform([](const std::optional<std::string>& s) {
                    if (!s) {
                        throw LispError("lambda: Arguments must be symbols.");
                    }
                    return *s;
                });
        std::vector<std::string> lambdaParams(viewLambdaParams.begin(), viewLambdaParams.end());
        auto lambdaBody = std::vector(params.begin() + 1, params.end());
        return std::make_shared<LambdaValue>(env.shared_from_this(), std::move(lambdaParams), std::move(lambdaBody));
    }
}