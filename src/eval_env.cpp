//
// Created by timetraveler314 on 5/3/24.
//

#include <algorithm>
#include <iterator>
#include <ranges>
#include <utility>

#include "eval_env.h"
#include "error.h"
#include "builtins.h"
#include "forms.h"

EvalEnv::EvalEnv(std::shared_ptr<EvalEnv> parent): parent{std::move(parent)}, symbolTable() {
    for (const auto& [name, value] : Builtins::builtinMap) {
        defineBinding(name, value);
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
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

ValuePtr EvalEnv::apply(const ValuePtr& proc, const std::vector<ValuePtr>& args) {
    if (!proc->is<ProcedureValue>()) {
        throw LispError("Not a procedure: " + proc->toString());
    }
    return std::dynamic_pointer_cast<ProcedureValue>(proc)->apply(args);
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

std::shared_ptr<EvalEnv>
EvalEnv::createChild(const std::vector<std::string> &params, const std::vector<ValuePtr> &args) {
    if (params.size() != args.size()) {
        throw LispError("Child EvalEnv parameter count mismatch.");
    }
    auto child = std::shared_ptr<EvalEnv>(shared_from_this());
    for (size_t i = 0; i < params.size(); ++i) {
        child->defineBinding(params[i], args[i]);
    }
    return child;
}
