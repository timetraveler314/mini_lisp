//
// Created by timetraveler314 on 5/12/24.
//

#ifndef MINI_LISP_UTILS_H
#define MINI_LISP_UTILS_H

#include <vector>
#include <string>
#include <format>
#include "../value.h"
#include "../error.h"

namespace Utils {
    bool isFalse(const ValuePtr& value);

    void checkParams(const std::string &name, size_t exact, const std::vector<ValuePtr> &params);
    void checkParams(const std::string &name, size_t min, size_t max, const std::vector<ValuePtr> &params);

    template<typename T>
    concept ValidTypePredicate = requires(T t, const ValuePtr& value) {
        { t(value) } -> std::convertible_to<bool>;
        { t.name } -> std::convertible_to<std::string>;
    };

    template<typename T>
    concept ValidTypeResolver = requires(T t, const ValuePtr& value) {
        requires ValidTypePredicate<T>;
        { t.resolve(value) } -> std::convertible_to<typename T::resolve_type>;
    };

    template<size_t index>
    void requireParamsImpl(const std::string& name, std::size_t total, const std::vector<ValuePtr>& params) {
        if (index != params.size()) {
            throw LispError(std::format("{}: expected {} arguments, but got {}", name, index, params.size()));
        }
    }

    template<size_t index, ValidTypePredicate Pred, ValidTypePredicate... Rest>
    void requireParamsImpl(const std::string& name, std::size_t total, const std::vector<ValuePtr>& params, Pred pred, Rest... preds) {
        if (index >= params.size()) throw LispError(std::format("{}: expected {} arguments, but got {}", name, total, params.size()));
        if (!pred(params[index])) throw LispError(std::format("{}: expected argument {} to be of type \"{}\"", name, index + 1, pred.name));
        requireParamsImpl<index + 1, Rest...>(name, total, params, preds...);
    }

    template<ValidTypePredicate... Preds>
    void requireParams(const std::string& name, const std::vector<ValuePtr>& params, Preds... preds) {
        requireParamsImpl<0, Preds...>(name, sizeof...(preds), params, preds...);
    }

    template<size_t index>
    std::tuple<> resolveParamsImpl(const std::string& name, std::size_t total, const std::vector<ValuePtr>& params) {
        return {};
    }

    template<size_t index, ValidTypeResolver Resolver, ValidTypeResolver... Rest>
    auto resolveParamsImpl(const std::string& name, std::size_t total, const std::vector<ValuePtr>& params, Resolver res, Rest... rest) {
        if (index >= params.size()) throw LispError(std::format("{}: expected {} arguments, but got {}", name, total, params.size()));
        if (!res(params[index])) throw LispError(std::format("{}: expected argument {} to be of type \"{}\"", name, index + 1, res.name));
        return std::tuple_cat(std::tuple(res.resolve(params[index])), resolveParamsImpl<index + 1, Rest...>(name, total, params, rest...));
    }

    template<ValidTypeResolver... Resolvers>
    auto resolveParams(const std::string& name, const std::vector<ValuePtr>& params, Resolvers... resolvers) {
        if (params.size() != sizeof...(resolvers)) throw LispError(std::format("{}: expected {} arguments, but got {}", name, sizeof...(resolvers), params.size()));
        return resolveParamsImpl<0, Resolvers...>(name, sizeof...(resolvers), params, resolvers...);
    }

    template<ValidTypeResolver Resolver>
    auto resolveAllParams(const std::string& name, const std::vector<ValuePtr>& params, Resolver res) {
        std::vector<typename Resolver::resolve_type> result;
        for (size_t i = 0; i < params.size(); ++i) {
            if (!res(params[i])) throw LispError(std::format("{}: expected argument {} to be of type \"{}\"", name, i + 1, res.name));
            result.push_back(res.resolve(params[i]));
        }
        return result;
    }

    struct IsAny {
        using resolve_type = const ValuePtr&;
        std::string name = "any";
        bool operator()(const ValuePtr& value) const {
            return true;
        }

        const ValuePtr& resolve(const ValuePtr& value) const {
            return value;
        }
    };
    static constexpr auto isAny = IsAny();

    struct IsNumeric {
        using resolve_type = double;
        std::string name = "number";
        bool operator()(const ValuePtr& value) const {
            return value->is<NumericValue>();
        }

        double resolve(const ValuePtr& value) const {
            return value->as<NumericValue>().value();
        }
    };
    static constexpr auto isNumeric = IsNumeric();

    struct IsInteger {
        using resolve_type = int;
        std::string name = "integer";
        bool operator()(const ValuePtr& value) const {
            return value->isNumericInteger();
        }

        int resolve(const ValuePtr& value) const {
            return value->asInteger().value();
        }
    };
    static constexpr auto isInteger = IsInteger();

    struct IsBoolean {
        using resolve_type = bool;
        std::string name = "boolean";
        bool operator()(const ValuePtr& value) const {
            return value->is<BooleanValue>();
        }

        bool resolve(const ValuePtr& value) const {
            return value->as<BooleanValue>().value();
        }
    };
    static constexpr auto isBoolean = IsBoolean();

    struct IsString {
        using resolve_type = std::string;
        std::string name = "string";
        bool operator()(const ValuePtr& value) const {
            return value->is<StringValue>();
        }

        std::string resolve(const ValuePtr& value) const {
            return value->as<StringValue>().value();
        }
    };
    static constexpr auto isString = IsString();

    struct IsSymbol {
        using resolve_type = std::string;
        std::string name = "symbol";
        bool operator()(const ValuePtr& value) const {
            return value->is<SymbolValue>();
        }

        std::string resolve(const ValuePtr& value) const {
            return value->asSymbol().value();
        }
    };
    static constexpr auto isSymbol = IsSymbol();

    struct IsPair {
        using resolve_type = std::shared_ptr<PairValue>;
        std::string name = "pair";
        bool operator()(const ValuePtr& value) const {
            return value->is<PairValue>();
        }

        std::shared_ptr<PairValue> resolve(const ValuePtr& value) const {
            return std::dynamic_pointer_cast<PairValue>(value);
        }
    };
    static constexpr auto isPair = IsPair();

    struct IsList {
        using resolve_type = std::vector<ValuePtr>;
        std::string name = "list";
        bool operator()(const ValuePtr& value) const {
            return value->isList();
        }

        std::vector<ValuePtr> resolve(const ValuePtr& value) const {
            return value->toVector();
        }
    };
    static constexpr auto isList = IsList();

    struct IsNonEmptyList {
        using resolve_type = std::vector<ValuePtr>;
        std::string name = "non-empty list";
        bool operator()(const ValuePtr& value) const {
            return value->isNonEmptyList();
        }

        std::vector<ValuePtr> resolve(const ValuePtr& value) const {
            return value->toVector();
        }
    };
    static constexpr auto isNonEmptyList = IsNonEmptyList();

    struct IsProcedure {
        using resolve_type = std::shared_ptr<ProcedureValue>;
        std::string name = "procedure";
        bool operator()(const ValuePtr& value) const {
            return value->is<ProcedureValue>();
        }

        std::shared_ptr<ProcedureValue> resolve(const ValuePtr& value) const {
            return std::dynamic_pointer_cast<ProcedureValue>(value);
        }
    };
    static constexpr auto isProcedure = IsProcedure();
}

#endif //MINI_LISP_UTILS_H
