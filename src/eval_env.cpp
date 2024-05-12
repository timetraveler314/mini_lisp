//
// Created by timetraveler314 on 5/3/24.
//

#include <algorithm>
#include <iterator>
#include <ranges>

#include "eval_env.h"
#include "error.h"
#include "builtins.h"
#include "forms.h"

EvalEnv::EvalEnv(): symbolTable() {
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
        auto value = lookup(*symbol);
        if (!value) {
            throw LispError("Variable " + *symbol + " not defined.");
        }
        return *value;
    }
    if (expr->isPair()) {
        auto* pair = dynamic_cast<PairValue*>(expr.get());
        if (auto name = pair->getCar()->asSymbol()) {
            if (SpecialForms::SPECIAL_FORMS.find(*name) != SpecialForms::SPECIAL_FORMS.end()) { // Special form
                return SpecialForms::SPECIAL_FORMS.at(*name)(pair->getCdr()->toVector(), *this);
            } else { // Not special form, treat as built-in procedure call now
                auto proc = eval(pair->getCar());
                auto args = pair->getCdr()->toVector()
                        | std::views::transform([this](ValuePtr v) { return this->eval(v); });
                return this->apply(proc, std::vector<ValuePtr>(args.begin(), args.end()));
            }
        } else { // TODO: When will this happen?
            throw LispError("Invalid expression.");
        }
    }
    throw LispError("Unimplemented");
}

ValuePtr EvalEnv::apply(ValuePtr proc, std::vector<ValuePtr> args) {
    if (proc->isBuiltinProc()) {
        return dynamic_cast<BuiltinProcValue*>(proc.get())->call(args);
    } else {
        throw LispError("Unimplemented");
    }
}

std::optional<ValuePtr> EvalEnv::lookup(const std::string& symbol) const {
    auto it = symbolTable.find(symbol);
    if (it == symbolTable.end()) {
        return std::nullopt;
    }
    return it->second;
}

void EvalEnv::addSymbol(const std::string &symbol, ValuePtr value) {
    symbolTable[symbol] = std::move(value);
}