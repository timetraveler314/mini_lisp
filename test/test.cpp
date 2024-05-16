//
// Created by timetraveler314 on 5/3/24.
//

#include "gtest/gtest.h"

#include <vector>

#include "../src/value.h"
#include "../src/token.h"
#include "../src/tokenizer.h"
#include "../src/error.h"

#include "test_builtins.cpp"
#include "test_special_forms.cpp"

TEST(ValueTest, IsTemplate){
    // NilValue
    auto nilValue = std::make_shared<NilValue>();
    EXPECT_TRUE(nilValue->is<NilValue>());
    EXPECT_FALSE(nilValue->is<NumericValue>());

    // NumericValue
    auto numericValue = std::make_shared<NumericValue>(1.0);
    EXPECT_TRUE(numericValue->is<NumericValue>());
    EXPECT_FALSE(numericValue->is<BooleanValue>());

    // BooleanValue
    auto booleanValue = std::make_shared<BooleanValue>(true);
    EXPECT_TRUE(booleanValue->is<BooleanValue>());
    EXPECT_FALSE(booleanValue->is<StringValue>());

    // StringValue
    auto stringValue = std::make_shared<StringValue>("hello");
    EXPECT_TRUE(stringValue->is<StringValue>());
    EXPECT_FALSE(stringValue->is<SymbolValue>());

    // SymbolValue
    auto symbolValue = std::make_shared<SymbolValue>("world");
    EXPECT_TRUE(symbolValue->is<SymbolValue>());
    EXPECT_FALSE(symbolValue->is<PairValue>());

    // SelfEvaluatingValue

}

TEST(ValueTest, ToString) {
    // NumericValue
    for (int i = -10000; i < 10000; i+= 1) {
        auto value = NumericValue(static_cast<double>(i));
        ASSERT_EQ(value.toString(), std::to_string(i));
    }

    for (int i = 9999999; i < 10001000; i+= 1) {
        auto value = NumericValue(static_cast<double>(i));
        ASSERT_EQ(value.toString(), std::to_string(i));
    }
}

TEST(ValueTest, AsInteger) {
    // NumericValue
    for (int i = -10000; i < 10000; i+= 1) {
        auto value = NumericValue(static_cast<double>(i));
        ASSERT_EQ(value.asInteger(), i);
    }

    auto doubleValue = NumericValue(1.5);
    EXPECT_EQ(doubleValue.asInteger(), std::nullopt);
}

TEST(ValueTest, ToVector) {
    // NilValue.toVector() should return an empty vector
    auto nilResult = std::make_shared<NilValue>()->toVector();
    EXPECT_TRUE(nilResult.empty());

    // toVector . fromVector = id
    std::vector<ValuePtr> nonEmptyVector = {
            std::make_shared<NumericValue>(1.0),
            std::make_shared<BooleanValue>(true),
            std::make_shared<StringValue>("hello"),
            std::make_shared<SymbolValue>("world"),
    };
    ValuePtr resultNonEmpty = Value::fromVector(nonEmptyVector);
    auto pairResult = resultNonEmpty->toVector();
    EXPECT_EQ(pairResult.size(), 4);
    EXPECT_EQ(pairResult[0]->as<NumericValue>(), 1.0);
    EXPECT_EQ(pairResult[1]->getType(), ValueType::BOOLEAN_VALUE);

    // Malformed list should throw a LispError
    EXPECT_THROW({
        auto malformedList = std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0));
        malformedList->toVector();
    }, LispError);
}

TEST(ValueTest, FromVector) {
    // Value::fromVector
    // Empty
    std::vector<ValuePtr> emptyVector;
    ValuePtr resultEmpty = Value::fromVector(emptyVector);
    EXPECT_EQ(resultEmpty->getType(), ValueType::NIL_VALUE);

    // Non-empty
    std::vector<ValuePtr> nonEmptyVector = {
            std::make_shared<NumericValue>(1.0),
            std::make_shared<BooleanValue>(true),
            std::make_shared<StringValue>("hello"),
            std::make_shared<SymbolValue>("world"),
    };
    ValuePtr resultNonEmpty = Value::fromVector(nonEmptyVector);
    EXPECT_EQ(resultNonEmpty->getType(), ValueType::PAIR_VALUE);
    EXPECT_EQ(resultNonEmpty->toString(), "(1 #t \"hello\" world)");
}

TEST(UtilsTest, RequireParams) {
    std::vector<ValuePtr> params = {
            std::make_shared<NumericValue>(1.0),
            std::make_shared<BooleanValue>(true),
            std::make_shared<StringValue>("hello"),
            std::make_shared<SymbolValue>("world"),
    };

    EXPECT_NO_THROW(
        Utils::requireParams("RequireParams", params, Utils::isInteger, Utils::isBoolean, Utils::isString, Utils::isSymbol));

    try {
        Utils::requireParams("RequireParams", params, Utils::isNumeric, Utils::isBoolean, Utils::isString);
    } catch (const LispError& e) {
        EXPECT_EQ(std::string(e.what()), "RequireParams: expected 3 arguments, but got 4");
    }

    try {
        Utils::requireParams("RequireParams", params, Utils::isNumeric, Utils::isNumeric, Utils::isNumeric, Utils::isNumeric);
    } catch (const LispError& e) {
        EXPECT_EQ(std::string(e.what()), "RequireParams: expected argument 2 to be of type \"number\"");
    }
}

TEST(UtilsTest, ResolveParams) {
    std::vector<ValuePtr> params = {
            std::make_shared<NumericValue>(1.0),
            std::make_shared<BooleanValue>(true),
            std::make_shared<StringValue>("hello"),
            std::make_shared<SymbolValue>("world"),
    };
    auto [integer1, boolean1, string1, symbol1] = Utils::resolveParams("RequireParams", params, Utils::isInteger, Utils::isBoolean, Utils::isString, Utils::isSymbol);
    EXPECT_EQ(integer1, 1);
    EXPECT_EQ(boolean1, true);
    EXPECT_EQ(string1, "hello");
    EXPECT_EQ(symbol1, "world");
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}