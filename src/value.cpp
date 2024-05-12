//
// Created by timetraveler314 on 5/3/24.
//

#include "value.h"

#include <cmath>
#include <iomanip>

#include "error.h"

bool Value::isSelfEvaluating() const {
    return getType() == ValueType::NUMERIC_VALUE
           || getType() == ValueType::BOOLEAN_VALUE
           || getType() == ValueType::STRING_VALUE
           || getType() == ValueType::BUILTIN_PROC_VALUE;
}

bool Value::isAtomic() const {
    return getType() == ValueType::NUMERIC_VALUE
           || getType() == ValueType::BOOLEAN_VALUE
           || getType() == ValueType::STRING_VALUE
           || getType() == ValueType::NIL_VALUE
           || getType() == ValueType::SYMBOL_VALUE;
}

bool Value::isBoolean() const {
    return getType() == ValueType::BOOLEAN_VALUE;
}

bool Value::isNumber() const {
    return getType() == ValueType::NUMERIC_VALUE;
}

bool Value::isNumericInteger() const {
    if (!isNumber()) return false;

    double intpart;
    return std::modf(dynamic_cast<const NumericValue*>(this)->getValue(), &intpart) == 0.0;
}

bool Value::isString() const {
    return getType() == ValueType::STRING_VALUE;
}

bool Value::isNil() const {
    return getType() == ValueType::NIL_VALUE;
}

bool Value::isPair() const {
    return getType() == ValueType::PAIR_VALUE;
}

bool Value::isSymbol() const {
    return getType() == ValueType::SYMBOL_VALUE;
}

bool Value::isBuiltinProc() const {
    return getType() == ValueType::BUILTIN_PROC_VALUE;
}

bool Value::isNonEmptyList() const {
    if (!isPair()) return false;

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
    return isNil() || isNonEmptyList();
}

std::optional<std::string> Value::asSymbol() const {
    if (isSymbol()) {
        return dynamic_cast<const SymbolValue*>(this)->getValue();
    } else {
        return std::nullopt;
    }
}

std::optional<double> Value::asNumber() const {
    if (isNumber()) {
        return dynamic_cast<const NumericValue*>(this)->getValue();
    } else {
        return std::nullopt;
    }
}

std::optional<bool> Value::asBoolean() const {
    if (isBoolean()) {
        return dynamic_cast<const BooleanValue*>(this)->getValue();
    } else {
        return std::nullopt;
    }
}

std::optional<int> Value::asInteger() const {
    double intpart;
    if (isNumber() && std::modf(dynamic_cast<const NumericValue*>(this)->getValue(), &intpart) == 0.0) {
        return std::lround(intpart);
    } else {
        return std::nullopt;
    }
}

std::optional<std::string> Value::asString() const {
    if (isString()) {
        return dynamic_cast<const StringValue *>(this)->getValue();
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
    if (isNil()) return result;
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

/**
 * @brief 将 NumericValue 转换为字符串表示形式
 *
 * 将 NumericValue
 * 对象转换为字符串表示形式，如果该值为整数，则返回整数部分的字符串形式，
 * 否则返回浮点数本身的字符串形式。
 *
 * @return 转换后的字符串
 *
 * @note 这里采用`std::modf`函数来判断一个浮点数的小数部分是否为0。
 */
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

template<>
std::string SymbolValue::toString() const {
    return value;
}

std::string NilValue::toString() const {
    return "()";
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
            if (auto nil = dynamic_cast<const NilValue*>(current->cdr.get())) {
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

std::string LambdaValue::toString() const {
    return "#<procedure>";
}
