//
// Created by timetraveler314 on 5/3/24.
//

#ifndef MINI_LISP_EVAL_ENV_H
#define MINI_LISP_EVAL_ENV_H

#include "value.h"

class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
    std::shared_ptr<EvalEnv> parent;
    std::unordered_map<std::string, ValuePtr> symbolTable;

    explicit EvalEnv(std::shared_ptr<EvalEnv> parent);

public:
    inline static std::shared_ptr<EvalEnv> createGlobal() {
        return std::shared_ptr<EvalEnv>(new EvalEnv(nullptr));
    }
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);

    void addSymbol(const std::string& symbol, ValuePtr value);

    ValuePtr eval(ValuePtr expr);
    ValuePtr apply(const ValuePtr& proc, const std::vector<ValuePtr>& args);

    // std::vector<ValuePtr> evalList(ValuePtr expr);

    std::optional<ValuePtr> lookupBinding(const std::string& symbol) const;
    void defineBinding(const std::string& symbol, ValuePtr value);

    std::vector<ValuePtr> evalList(ValuePtr expr);
};

#endif //MINI_LISP_EVAL_ENV_H
