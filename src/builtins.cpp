//
// Created by timetraveler314 on 5/4/24.
//

#include <numeric>
#include "builtins.h"

const std::unordered_map<std::string, ValuePtr> Builtins::builtinMap = {
        {"+", std::make_shared<BuiltinProcValue>(add)},
};

// (+ n1 n2 ... nk)
ValuePtr Builtins::add(const std::vector<ValuePtr>& params) {
    double result = 0;
    for (const auto& i : params) {
        if (!i->isNumber()) {
            throw LispError("Cannot add a non-numeric value.");
        }
        result += *(i->asNumber());
    }
    return std::make_shared<NumericValue>(result);
}