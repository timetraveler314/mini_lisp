//
// Created by timetraveler314 on 5/3/24.
//

#include <algorithm>
#include <iterator>
#include <ranges>

#include "eval_env.h"
#include "error.h"
#include "builtins.h"

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
    if (expr->isNonEmptyList()) {
        using namespace std::literals;
        auto list = expr->toVector();
        if (list[0]->asSymbol() == "define"s) {
            if (list.size() != 3) {
                throw LispError("define: Invalid number of arguments.");
            }
            auto symbol = list[1]->asSymbol();
            if (!symbol) {
                throw LispError("define: Invalid symbol.");
            }
            auto value = list[2];

            symbolTable[*symbol] = eval(value);
            return std::make_shared<NilValue>();
        } else { // Not special form
            auto proc = eval(list[0]);
            auto args = list | std::views::drop(1)
                    | std::views::transform([this](ValuePtr v) { return this->eval(v); });
            return this->apply(proc, std::vector<ValuePtr>(args.begin(), args.end()));
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