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
    LAMBDA_VALUE,
    CUSTOM_VALUE,
};

class Value;

using ValuePtr = std::shared_ptr<Value>;
using BuiltinFuncType = std::function<ValuePtr(const std::vector<ValuePtr>&)>;

template<typename T>
concept IsConcreteValue = requires(T t) {
    std::is_base_of_v<Value, T>;
    { t.getValue() } -> std::convertible_to<typename T::element_type>;
};

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

    template<class T> requires std::is_base_of_v<Value, T>
    constexpr bool is() const {
        return dynamic_cast<const T*>(this) != nullptr;
    }

    template<IsConcreteValue T>
    auto as() const -> std::optional<typename T::element_type> {
        if (is<T>()) {
            return dynamic_cast<const T*>(this)->getValue();
        } else {
            return std::nullopt;
        }
    }

    bool isNumericInteger() const;
    bool isNonEmptyList() const;
    bool isList() const;

    std::optional<std::string> asSymbol() const;
    std::optional<int> asInteger() const;

    static ValuePtr fromVector(const std::vector<ValuePtr>& values);
};

class SelfEvaluatingValue : virtual public Value {};

class AtomicValue : virtual public Value {};

template<ValueType value_type, typename T>
class ConcreteValue;

using BooleanValue = ConcreteValue<ValueType::BOOLEAN_VALUE, bool>;
using NumericValue = ConcreteValue<ValueType::NUMERIC_VALUE, double>;
using StringValue = ConcreteValue<ValueType::STRING_VALUE, std::string>;

template<ValueType value_type, typename T>
class ConcreteValue final : public SelfEvaluatingValue, public AtomicValue {
    T value;

public:
    using element_type = T;

    explicit ConcreteValue(T value): Value(value_type), value{value} {}

    T getValue() const {
        return value;
    }

    std::string toString() const override;
};

class NilValue final : public AtomicValue {
public:
    NilValue(): Value(ValueType::NIL_VALUE) {}

    std::string toString() const override;
};

class SymbolValue final : public AtomicValue {
    std::string name;

public:
    explicit SymbolValue(std::string name): Value(ValueType::SYMBOL_VALUE), name{std::move(name)} {}

    inline std::string getValue() const {
        return name;
    }

    std::string toString() const override;
};

class PairValue final : public Value {
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

class ProcedureValue : virtual public Value {
public:
    virtual ValuePtr apply(const std::vector<ValuePtr>& args) = 0;
};

class BuiltinProcValue final : public ProcedureValue {
    BuiltinFuncType func;

public:
    explicit BuiltinProcValue(BuiltinFuncType func): Value(ValueType::BUILTIN_PROC_VALUE), func{std::move(func)} {}

    inline ValuePtr apply(const std::vector<ValuePtr>& params) override {
        return func(params);
    }

    inline std::string toString() const override {
        return "#<procedure>";
    }
};

class EvalEnv;

class LambdaValue final : public ProcedureValue {
private:
    std::shared_ptr<EvalEnv> env;
    std::vector<std::string> params;
    std::vector<ValuePtr> body;

public:
    LambdaValue(std::shared_ptr<EvalEnv> env, std::vector<std::string> params, std::vector<ValuePtr> body): Value(ValueType::LAMBDA_VALUE), env{std::move(env)}, params{std::move(params)}, body{std::move(body)} {}

    std::string toString() const override;

    ValuePtr apply(const std::vector<ValuePtr>& args) override;
};

#endif //MINI_LISP_VALUE_H
