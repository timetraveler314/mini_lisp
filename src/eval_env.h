//
// Created by timetraveler314 on 5/3/24.
//

#ifndef MINI_LISP_EVAL_ENV_H
#define MINI_LISP_EVAL_ENV_H

#include <stack>

#include "value.h"

class EvalEnv : public std::enable_shared_from_this<EvalEnv> {
    std::optional<std::string> name;

    std::shared_ptr<EvalEnv> parent;
    std::unordered_map<std::string, ValuePtr> symbolTable;

    std::stack<ValuePtr> evalStack;

    explicit EvalEnv(std::shared_ptr<EvalEnv> parent);
    ValuePtr eval_impl(ValuePtr expr);

public:
    static std::shared_ptr<EvalEnv> createGlobal() {
        return std::shared_ptr<EvalEnv>(new EvalEnv(nullptr));
    }

    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args);
    std::shared_ptr<EvalEnv> createChild(const std::vector<std::string>& params, const std::vector<ValuePtr>& args, const std::string& name);

    bool isGlobal() const {
        return parent == nullptr;
    }

    void reset();

    ValuePtr eval(ValuePtr expr);

    std::vector<ValuePtr> evalList(ValuePtr expr);

    ValuePtr apply(const ValuePtr& proc, const std::vector<ValuePtr>& args);

    std::optional<ValuePtr> lookupBinding(const std::string& symbol) const;
    void defineBinding(const std::string& symbol, ValuePtr value);

    bool isStackEmpty() const {
        return evalStack.empty();
    }
    std::string generateStackTrace(int depth);
    void clearStack();
    void pushStack(ValuePtr value);
    void popStack();

    std::string getName() const {
        return name.value_or("(unnamed)");
    }
};

#endif //MINI_LISP_EVAL_ENV_H
