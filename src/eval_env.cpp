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
        symbolTable[name] = value;
    }
}

ValuePtr EvalEnv::eval(ValuePtr expr) {
    if (expr->isSelfEvaluating()) {
        return expr;
    }
    if (expr->isNil()) {
        throw LispError("Evaluating nil is prohibited.");
    }
    if (auto symbol = expr->asSymbol()) {
        auto value = lookupBinding(*symbol);
        if (!value) {
            throw LispError("Variable " + *symbol + " not defined.");
        }
        return *value;
    }
    if (expr->isPair()) {
        if (!expr->isList()) throw LispError("Malformed list " + expr->toString());
        auto* pair = dynamic_cast<PairValue*>(expr.get());
        if (auto name = pair->getCar()->asSymbol()) {
            if (SpecialForms::SPECIAL_FORMS.find(*name) != SpecialForms::SPECIAL_FORMS.end()) { // Special form
                return SpecialForms::SPECIAL_FORMS.at(*name)(pair->getCdr()->toVector(), *this);
            }
        }
        // Not special form, treat as built-in procedure call or lambda call
        auto car = eval(pair->getCar());
        if (car->isBuiltinProc() || car->isLambda()) {
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
    if (proc->isBuiltinProc()) {
        return dynamic_cast<BuiltinProcValue*>(proc.get())->call(args);
    } else if (proc->isLambda()) {
        auto lambda = dynamic_cast<LambdaValue*>(proc.get());
        return lambda->apply(args);
    } else {
        throw LispError("Unexpected procedure type.");
    }
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

void EvalEnv::addSymbol(const std::string &symbol, ValuePtr value) {
    symbolTable[symbol] = std::move(value);
}

void EvalEnv::defineBinding(const std::string &symbol, ValuePtr value) {
    if (parent && parent->lookupBinding(symbol)) {
        throw LispError("Cannot redefine binding in parent environment.");
    }
    symbolTable[symbol] = std::move(value);
}

std::shared_ptr<EvalEnv>
EvalEnv::createChild(const std::vector<std::string> &params, const std::vector<ValuePtr> &args) {
    if (params.size() != args.size()) {
        throw LispError("Child EvalEnv parameter count mismatch.");
    }
    auto child = std::shared_ptr<EvalEnv>(shared_from_this());
    for (size_t i = 0; i < params.size(); ++i) {
        child->addSymbol(params[i], args[i]);
    }
    return child;
}
