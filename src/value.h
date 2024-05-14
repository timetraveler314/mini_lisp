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
    BUILTIN_PROC_VALUE,
    LAMBDA_VALUE
};

class Value;
class PairValue;
using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = std::function<ValuePtr(const std::vector<ValuePtr>&)>;

class Value {
    ValueType type;

protected:
    explicit Value(ValueType type): type{type} {}

public:
    virtual ~Value() = default;

    inline ValueType getType() const {
        return type;
    }

    virtual std::string toString() const = 0;
    std::vector<ValuePtr> toVector();

    bool isSelfEvaluating() const;
    bool isAtomic() const;
    bool isBoolean() const;
    bool isNumber() const;
    bool isNumericInteger() const;
    bool isString() const;
    bool isNil() const;
    bool isPair() const;
    bool isSymbol() const;
    bool isBuiltinProc() const;
    bool isNonEmptyList() const;
    bool isList() const;
    bool isLambda() const;

    std::optional<std::string> asSymbol() const;
    std::optional<double> asNumber() const;
    std::optional<int> asInteger() const;
    std::optional<bool> asBoolean() const;
    std::optional<std::string> asString() const;

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

class BuiltinProcValue : public Value {
    BuiltinFuncType func;

public:
    explicit BuiltinProcValue(BuiltinFuncType func): Value(ValueType::BUILTIN_PROC_VALUE), func{std::move(func)} {}

    inline ValuePtr call(const std::vector<ValuePtr>& params) {
        return func(params);
    }

    inline std::string toString() const override {
        return "#<procedure>";
    }
};

class EvalEnv;

class LambdaValue : public Value {
private:
    std::shared_ptr<EvalEnv> env;
    std::vector<std::string> params;
    std::vector<ValuePtr> body;

public:
    LambdaValue(std::shared_ptr<EvalEnv> env, std::vector<std::string> params, std::vector<ValuePtr> body): Value(ValueType::LAMBDA_VALUE), env{std::move(env)}, params{std::move(params)}, body{std::move(body)} {}

    std::string toString() const override;

    ValuePtr apply(const std::vector<ValuePtr>& args);
};

#endif //MINI_LISP_VALUE_H
