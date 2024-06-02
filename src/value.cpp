//
// Created by timetraveler314 on 5/3/24.
//

#include "value.h"

#include <cmath>
#include <iomanip>

#include "error.h"
#include "eval_env.h"

bool Value::isNumericInteger() const {
    if (!is<NumericValue>()) return false;

    double intpart;
    return std::modf(dynamic_cast<const NumericValue*>(this)->getValue(), &intpart) == 0.0;
}

bool Value::isNonEmptyList() const {
    if (!is<PairValue>()) return false;

    auto current = dynamic_cast<const PairValue*>(this);
    while (current) {
        if (auto pair = dynamic_cast<const PairValue*>(current->getCdr().get())) {
            current = pair;
        } else {
            if (auto nil = dynamic_cast<const NilValue*>(current->getCdr().get())) {
                return true;
            } else {
                return false;
            }
        }
    }

    return false;
}

bool Value::isList() const {
    return is<NilValue>() || isNonEmptyList();
}

std::optional<std::string> Value::asSymbol() const {
    if (is<SymbolValue>()) {
        return dynamic_cast<const SymbolValue*>(this)->getValue();
    } else {
        return std::nullopt;
    }
}

std::optional<int> Value::asInteger() const {
    double intpart;
    if (is<NumericValue>() && std::modf(dynamic_cast<const NumericValue*>(this)->getValue(), &intpart) == 0.0) {
        return std::lround(intpart);
    } else {
        return std::nullopt;
    }
}

ValuePtr Value::fromVector(const std::vector<ValuePtr> &values) {
    ValuePtr result = std::make_shared<NilValue>();
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        result = std::make_shared<PairValue>(*it, result);
    }
    return result;
}

std::vector<ValuePtr> Value::toVector() {
    std::vector<ValuePtr> result;
    if (is<NilValue>()) return result;
    if (!isNonEmptyList()) throw LispError("Cannot convert improper list to vector.");

    auto current = dynamic_cast<const PairValue*>(this);
    while (true) {
        result.push_back(current->getCar());
        if (auto pair = dynamic_cast<const PairValue*>(current->getCdr().get())) {
            current = pair;
        } else {
            if (auto nil = dynamic_cast<const NilValue*>(current->getCdr().get())) {
                return result;
            } else {
                throw LispError("Cannot convert improper list to vector. This branch shall not happen.");
            }
        }
    }
}

template <>
std::string NumericValue::toString() const {
    double intpart;
    return std::modf(value, &intpart) == 0.0 ? std::to_string(std::lround(intpart)) : std::to_string(value);
}

template<>
std::string BooleanValue::toString() const {
    return value ? "#t" : "#f";
}

template<>
std::string StringValue::toString() const {
    std::ostringstream ss;
    ss << std::quoted(value);
    return ss.str();
}

std::string NilValue::toString() const {
    return "()";
}

std::string SymbolValue::toString() const {
    return name;
}

std::string PairValue::toString() const {
    std::ostringstream ss;
    ss << "(";
    auto current = this;

    while (current) {
        ss << current->car->toString();
        if (auto pair = dynamic_cast<const PairValue*>(current->cdr.get())) {
            ss << " ";
            current = pair;
        } else {
            if (dynamic_cast<const NilValue*>(current->cdr.get())) {
                ss << ")";
                current = nullptr;
            } else {
                ss << " . " << current->cdr->toString() << ")";
                current = nullptr;
            }
        }
    }

    return ss.str();
}

bool PairValue::isEqual(const ValuePtr &other) const {
    if (auto ptr = std::dynamic_pointer_cast<PairValue>(other)) {
        return car->isEqual(ptr->car) && cdr->isEqual(ptr->cdr);
    }
    return false;
}

std::string LambdaValue::toString() const {
    return "#<procedure>";
}

ValuePtr LambdaValue::apply(const std::vector<ValuePtr> &args) {
    if (args.size() != params.size()) {
        throw LispError("Procedure expected " + std::to_string(params.size()) + " arguments, but got " + std::to_string(args.size()));
    }

    auto lambdaEnv = env->createChild(params, args);
    ValuePtr evalResult;
    for (const auto& expr : body) {
        evalResult = lambdaEnv->eval(expr);
    }
    return evalResult;
}
