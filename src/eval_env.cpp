//
// Created by timetraveler314 on 5/3/24.
//

#include <algorithm>
#include <iterator>
#include <ranges>
#include <utility>

#include "eval_env.h"

#include <iostream>
#include <sstream>

#include "error.h"
#include "builtins.h"
#include "forms.h"

EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent): parent{std::move(parent)}, symbolTable() {
    for (const auto& [name, value] : Builtins::builtinMap) {
        defineBinding(name, value);
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    try {
        evalStack.emplace(expr);
        auto result = eval_impl(expr);
        evalStack.pop();
        return result;
    } catch (LispErrorWithEnv&) {
        throw; // Rethrow if already wrapped
    } catch (LispError& e) {
        throw LispErrorWithEnv(e.what(), shared_from_this()); // Wrap the error with the current environment
    }
}

ValuePtr EvalEnv::eval_impl(ValuePtr expr) {
    if (expr->is<SelfEvaluatingValue>()) {
        return expr;
    }
    if (expr->is<NilValue>()) {
        throw LispError("Evaluating nil is prohibited.");
    }
    if (auto symbol = expr->asSymbol()) {
        auto value = lookupBinding(*symbol);
        if (!value) {
            throw LispError("Variable " + *symbol + " not defined.");
        }
        return *value;
    }
    if (expr->is<PairValue>()) {
        if (!expr->isList()) throw LispError("Malformed list " + expr->toString());
        auto pair = std::dynamic_pointer_cast<PairValue>(expr);
        if (auto name = pair->getCar()->asSymbol()) {
            if (SpecialForms::SPECIAL_FORMS.find(*name) != SpecialForms::SPECIAL_FORMS.end()) { // Special form
                return SpecialForms::SPECIAL_FORMS.at(*name)(pair->getCdr()->toVector(), *this);
            }
        }
        // Not special form, treat as built-in procedure call or lambda call
        auto car = eval(pair->getCar());
        if (car->is<ProcedureValue>()) {
            auto args = pair->getCdr()->toVector()
                        | std::views::transform([this](ValuePtr v) { return this->eval(std::move(v)); });
            return this->apply(car, std::vector<ValuePtr>(args.begin(), args.end()));
        }
        // Not a procedure
        throw LispError("Not a procedure: " + car->toString());
    }
    throw LispError("Unimplemented");
}

std::vector<ValuePtr> EvalEnv::evalList(ValuePtr expr) {
    std::vector<ValuePtr> result;
    std::ranges::transform(expr->toVector(),
                           std::back_inserter(result),
                           [this](ValuePtr v) { return this->eval(v); });
    return result;
}

ValuePtr EvalEnv::apply(const ValuePtr& proc, const std::vector<ValuePtr>& args) {
    if (auto procValue = std::dynamic_pointer_cast<ProcedureValue>(proc)) {
        return procValue->apply(args, *this);
    }
    throw LispError("Not a procedure: " + proc->toString());
}

std::optional<ValuePtr> EvalEnv::lookupBinding(const std::string& symbol) const {
    if (symbolTable.find(symbol) != symbolTable.end()) {
        return symbolTable.at(symbol);
    }
    if (parent) {
        return parent->lookupBinding(symbol);
    }
    return std::nullopt;
}

void EvalEnv::defineBinding(const std::string &symbol, ValuePtr value) {
    symbolTable[symbol] = std::move(value);
}

std::string EvalEnv::generateStackTrace(int depth) {
    int count = 0;
    std::stringstream ss;

    std::shared_ptr<EvalEnv> env = shared_from_this();
    while (env && count < depth) {
        auto top = env->evalStack.top();
        env->evalStack.pop();

        if (auto pair = std::dynamic_pointer_cast<PairValue>(top)) {
            if (pair->position) {
                if (!env->isGlobal()) {
                    ss << std::format("In environment: {}\n", env->getName());
                }

                ss << std::format("At: line {}, column {}\n", pair->position->line, pair->position->column);

                if (pair->position->column > 3) {
                    ss << "  ... " << pair->toString() << std::endl;
                } else ss << "  " << pair->toString() << std::endl;
            }
        }

        count++;
        env = env->runtimeParent;
    }

    // while (!evalStack.empty()) {
    //     if (count++ >= depth) break;
    //     auto top = evalStack.top();
    //     evalStack.pop();
    //
    //     if (auto pair = std::dynamic_pointer_cast<PairValue>(top)) {
    //         if (pair->position) {
    //             ss << std::format("At: Line {}, column {}\n", pair->position->line, pair->position->column);
    //             ss << "  " << pair->toString() << std::endl;
    //         }
    //     }
    //
    //     std::cout << "Debug: " << top->toString() << std::endl;
    // }

    return ss.str();
}

void EvalEnv::clearStack() {
    while (!evalStack.empty()) evalStack.pop();

    if (parent) {
        parent->clearStack();
    }
}

void EvalEnv::pushStack(ValuePtr value) {
    auto env = shared_from_this();
    while (env) {
        env->evalStack.push(value);
        env = env->parent;
    }
}

void EvalEnv::popStack() {
    auto env = shared_from_this();
    while (env) {
        env->evalStack.pop();
        env = env->parent;
    }
}

std::shared_ptr<EvalEnv>
EvalEnv::createChild(const std::vector<std::string> &params, const std::vector<ValuePtr> &args, std::shared_ptr<EvalEnv> runtimeParent) {
    if (params.size() != args.size()) {
        throw LispError("Child EvalEnv parameter count mismatch.");
    }
    auto child = EvalEnv::createGlobal();
    child->parent = shared_from_this();
    child->runtimeParent = runtimeParent;
    for (size_t i = 0; i < params.size(); ++i) {
        child->defineBinding(params[i], args[i]);
    }
    return child;
}

std::shared_ptr<EvalEnv> EvalEnv::createChild(const std::vector<std::string> &params, const std::vector<ValuePtr> &args,
    const std::string &name, std::shared_ptr<EvalEnv> runtimeParent) {
    auto child = createChild(params, args, runtimeParent);
    child->name = name;
    return child;
}

void EvalEnv::reset() {
    symbolTable.clear();
    for (const auto& [name, value] : Builtins::builtinMap) {
        defineBinding(name, value);
    }
}
