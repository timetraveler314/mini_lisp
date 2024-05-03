//
// Created by timetraveler314 on 5/3/24.
//

#ifndef MINI_LISP_VALUE_H
#define MINI_LISP_VALUE_H

#include <functional>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

enum class ValueType {
    BOOLEAN_VALUE,
    NUMERIC_VALUE,
    STRING_VALUE,
    NIL_VALUE,
    SYMBOL_VALUE,
    PAIR_VALUE,
};

class Value;
using ValuePtr = std::shared_ptr<Value>;

class Value {
    ValueType type;

protected:
    explicit Value(ValueType type): type{type} {}

public:
    virtual ~Value() = default;

    ValueType getType() const {
        return type;
    }

    virtual std::string toString() const = 0;

    static ValuePtr fromVector(const std::vector<ValuePtr>& values);
};

template<ValueType value_type, typename T>
class PrimitiveValue;

using BooleanValue = PrimitiveValue<ValueType::BOOLEAN_VALUE, bool>;
using NumericValue = PrimitiveValue<ValueType::NUMERIC_VALUE, double>;
using StringValue = PrimitiveValue<ValueType::STRING_VALUE, std::string>;
using SymbolValue = PrimitiveValue<ValueType::SYMBOL_VALUE, std::string>;

template<ValueType value_type, typename T>
class PrimitiveValue : public Value {
    T value;

public:
    explicit PrimitiveValue(T value): Value(value_type), value{value} {}

    T getValue() const {
        return value;
    }

    std::string toString() const override;
};

class NilValue : public Value {
public:
    NilValue(): Value(ValueType::NIL_VALUE) {}

    std::string toString() const override;
};

class PairValue : public Value {
    ValuePtr car;
    ValuePtr cdr;

public:
    PairValue(const ValuePtr& car, const ValuePtr& cdr): Value(ValueType::PAIR_VALUE), car{car}, cdr{cdr} {}

    ValuePtr getCar() const {
        return car;
    }

    ValuePtr getCdr() const {
        return cdr;
    }

    std::string toString() const override;
};

#endif //MINI_LISP_VALUE_H
