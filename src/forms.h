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
    ValuePtr _and(const std::vector<ValuePtr>& params, EvalEnv& env);
    ValuePtr _or(const std::vector<ValuePtr>& params, EvalEnv& env);

    ValuePtr _lambda(const std::vector<ValuePtr>& params, EvalEnv& env);
}

#endif //MINI_LISP_FORMS_H
