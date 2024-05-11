//
// Created by timetraveler314 on 5/3/24.
//

#include "gtest/gtest.h"

#include <vector>

#include "../src/value.h"
#include "../src/token.h"
#include "../src/tokenizer.h"
#include "../src/error.h"

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
    EXPECT_EQ(pairResult[0]->asNumber(), 1.0);
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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}