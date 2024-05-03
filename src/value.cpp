//
// Created by timetraveler314 on 5/3/24.
//

#include "value.h"

#include <cmath>
#include <iomanip>

#include "error.h"

ValuePtr Value::fromVector(const std::vector<ValuePtr> &values) {
    ValuePtr result = std::make_shared<NilValue>();
    for (auto it = values.rbegin(); it != values.rend(); ++it) {
        result = std::make_shared<PairValue>(*it, result);
    }
    return result;
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