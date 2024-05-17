//
// Created by timetraveler314 on 5/12/24.
//

#ifndef MINI_LISP_FORMS_H
#define MINI_LISP_FORMS_H

#include <unordered_map>
#include <string>
#include "value.h"
#include "eval_env.h"

using SpecialFormType = std::function<ValuePtr(const std::vector<ValuePtr>&, EvalEnv&)>;

namespace SpecialForms {
    extern const std::unordered_map<std::string, SpecialFormType> SPECIAL_FORMS;

    ValuePtr _define(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _quote(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _if(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _cond(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _let(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _begin(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _quasiquote(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _unquote(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _and(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _or(const std::vector<ValuePtr>& params, EvalEnv& env);

    ValuePtr _lambda(const std::vector<ValuePtr>& params, EvalEnv& env);

    ValuePtr _quasiquote_impl(const ValuePtr& value, EvalEnv& env);
}

#endif //MINI_LISP_FORMS_H
