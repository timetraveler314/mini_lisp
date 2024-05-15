//
// Created by timetraveler314 on 5/12/24.
//

#include <gtest/gtest.h>

#include "../src/builtins.h"

TEST(BuiltinsTest, Display) {
    testing::internal::CaptureStdout();
    auto result = Builtins::_display({std::make_shared<StringValue>("hello")});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "hello");
    ASSERT_EQ(result->getType(), ValueType::NIL_VALUE);

    testing::internal::CaptureStdout();
    auto result2 = Builtins::_display({std::make_shared<NumericValue>(1.0)});
    std::string output2 = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output2, "1");
    ASSERT_EQ(result2->getType(), ValueType::NIL_VALUE);

    EXPECT_THROW(Builtins::_display({}), LispError);
}

TEST(BuiltinsTest, Displayln) {
    testing::internal::CaptureStdout();
    auto result = Builtins::_displayln({std::make_shared<StringValue>("hello")});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "hello\n");
    ASSERT_EQ(result->getType(), ValueType::NIL_VALUE);

    testing::internal::CaptureStdout();
    auto result2 = Builtins::_displayln({std::make_shared<NumericValue>(1.0)});
    std::string output2 = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output2, "1\n");
    ASSERT_EQ(result2->getType(), ValueType::NIL_VALUE);

    EXPECT_THROW(Builtins::_displayln({}), LispError);
}

TEST(BuiltinsTest, Error) {
    EXPECT_THROW(Builtins::_error({
                                          std::make_shared<StringValue>("error message")
                                  }), LispError);
}

TEST(BuiltinsTest, Exit) {
    EXPECT_EXIT(Builtins::_exit({}), ::testing::ExitedWithCode(0), "");
    EXPECT_EXIT(Builtins::_exit({std::make_shared<NumericValue>(1.0)}), ::testing::ExitedWithCode(1), "");
}

TEST(BuiltinsTest, Newline) {
    testing::internal::CaptureStdout();
    auto result = Builtins::_newline({});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "\n");
    ASSERT_EQ(result->getType(), ValueType::NIL_VALUE);

    EXPECT_THROW(Builtins::_newline({std::make_shared<NumericValue>(1.0)}), LispError);
}

TEST(BuiltinsTest, Print) {
    testing::internal::CaptureStdout();
    auto result = Builtins::_print({std::make_shared<StringValue>("hello")});
    std::string output = testing::internal::GetCapturedStdout();
    ASSERT_EQ(output, "\"hello\"\n");
    ASSERT_EQ(result->getType(), ValueType::NIL_VALUE);

    EXPECT_THROW(Builtins::_print({}), LispError);
}

TEST(BuiltinsTest, TypeCheckers) {
    auto atomChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("atom?"));
    ASSERT_TRUE(*(atomChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));
    ASSERT_TRUE(*(atomChecker->apply({std::make_shared<BooleanValue>(true)})->as<BooleanValue>()));
    ASSERT_TRUE(*(atomChecker->apply({std::make_shared<StringValue>("hello")})->as<BooleanValue>()));
    ASSERT_TRUE(*(atomChecker->apply({std::make_shared<SymbolValue>("world")})->as<BooleanValue>()));
    ASSERT_TRUE(*(atomChecker->apply({std::make_shared<NilValue>()})->as<BooleanValue>()));
    ASSERT_FALSE(*(atomChecker->apply({std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0))})->as<BooleanValue>()));

    auto booleanChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("boolean?"));
    ASSERT_TRUE(*(booleanChecker->apply({std::make_shared<BooleanValue>(true)})->as<BooleanValue>()));
    ASSERT_FALSE(*(booleanChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));

    auto integerChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("integer?"));
    ASSERT_TRUE(*(integerChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));
    ASSERT_FALSE(*(integerChecker->apply({std::make_shared<NumericValue>(1.5)})->as<BooleanValue>()));
    ASSERT_FALSE(*(integerChecker->apply({std::make_shared<BooleanValue>(true)})->as<BooleanValue>()));

    auto listChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("list?"));
    ASSERT_TRUE(*(listChecker->apply({std::make_shared<NilValue>()})->as<BooleanValue>()));
    ASSERT_TRUE(*(listChecker->apply({std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NilValue>())})->as<BooleanValue>()));
    ASSERT_FALSE(*(listChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));

    auto numberChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("number?"));
    ASSERT_TRUE(*(numberChecker->apply({std::make_shared<NumericValue>(1.5)})->as<BooleanValue>()));
    ASSERT_FALSE(*(numberChecker->apply({std::make_shared<BooleanValue>(true)})->as<BooleanValue>()));

    auto pairChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("pair?"));
    ASSERT_TRUE(*(pairChecker->apply({std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NilValue>())})->as<BooleanValue>()));
    ASSERT_FALSE(*(pairChecker->apply({std::make_shared<NilValue>()})->as<BooleanValue>()));

    auto nullChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("null?"));
    ASSERT_TRUE(*(nullChecker->apply({std::make_shared<NilValue>()})->as<BooleanValue>()));
    ASSERT_FALSE(*(nullChecker->apply({std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NilValue>())})->as<BooleanValue>()));

    auto stringChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("string?"));
    ASSERT_TRUE(*(stringChecker->apply({std::make_shared<StringValue>("hello")})->as<BooleanValue>()));
    ASSERT_FALSE(*(stringChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));

    auto symbolChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("symbol?"));
    ASSERT_TRUE(*(symbolChecker->apply({std::make_shared<SymbolValue>("world")})->as<BooleanValue>()));
    ASSERT_FALSE(*(symbolChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));

    auto procedureChecker = std::dynamic_pointer_cast<BuiltinProcValue>(Builtins::builtinMap.at("procedure?"));
    ASSERT_TRUE(*(procedureChecker->apply({std::make_shared<BuiltinProcValue>(Builtins::_add)})->as<BooleanValue>()));
    ASSERT_FALSE(*(procedureChecker->apply({std::make_shared<NumericValue>(1.0)})->as<BooleanValue>()));
}

TEST(BuiltinsTest, Car) {
    auto result = Builtins::_car({std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0))});
    ASSERT_EQ(result->as<NumericValue>(), 1.0);
    EXPECT_THROW(Builtins::_car({std::make_shared<NilValue>()}), LispError);
}

TEST(BuiltinsTest, Cdr) {
    auto result = Builtins::_cdr({std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0))});
    ASSERT_EQ(result->as<NumericValue>(), 2.0);
    EXPECT_THROW(Builtins::_cdr({std::make_shared<NilValue>()}), LispError);
}

TEST(BuiltinsTest, Cons) {
    auto result = Builtins::_cons({std::make_shared<NumericValue>(1.0),
            std::make_shared<PairValue>(std::make_shared<NumericValue>(2.0), std::make_shared<NilValue>())});
    ASSERT_EQ(result->toString(), "(1 2)");
    auto result2 = Builtins::_cons({std::make_shared<NumericValue>(1.0), std::make_shared<NilValue>()});
    ASSERT_EQ(result2->toString(), "(1)");
}

TEST(BuiltinsTest, Length) {
    auto result = Builtins::_length({
        std::make_shared<PairValue>(std::make_shared<NumericValue>(1.0),std::make_shared<PairValue>(std::make_shared<NumericValue>(2.0), std::make_shared<NilValue>()))
    });
    EXPECT_EQ(result->as<NumericValue>(), 2.0);
    auto result2 = Builtins::_length({std::make_shared<NilValue>()});
    EXPECT_EQ(result2->as<NumericValue>(), 0.0);
}

TEST(BuiltinsTest, List) {
    auto result = Builtins::_list({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_EQ(result->toString(), "(1 2)");
}

TEST(BuiltinsTest, Add) {
    auto resultZero = Builtins::_add({});
    ASSERT_EQ(resultZero->as<NumericValue>(), 0.0);
    auto result2 = Builtins::_add({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_EQ(result2->as<NumericValue>(), 3.0);
    auto result4 = Builtins::_add({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0),
                                   std::make_shared<NumericValue>(3.0), std::make_shared<NumericValue>(4.0)});
    ASSERT_EQ(result4->as<NumericValue>(), 10.0);
}

TEST(BuiltinsTest, Sub) {
    auto result = Builtins::_sub({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_EQ(result->as<NumericValue>(), -1.0);
    auto result2 = Builtins::_sub({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_EQ(result2->as<NumericValue>(), -1.0);
}

TEST(BuiltinsTest, Mul) {
    auto result = Builtins::_mul({});
    ASSERT_EQ(result->as<NumericValue>(), 1.0);
    auto result2 = Builtins::_mul({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0),
                                   std::make_shared<NumericValue>(3.0), std::make_shared<NumericValue>(4.0)});
    ASSERT_EQ(result2->as<NumericValue>(), 24.0);
}

TEST(BuiltinsTest, Div) {
    auto result = Builtins::_div({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_EQ(result->as<NumericValue>(), 0.5);
    EXPECT_THROW(Builtins::_div({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(0.0)}), LispError);
}

TEST(BuiltinsTest, Abs) {
    auto result = Builtins::_abs({std::make_shared<NumericValue>(-1.0)});
    ASSERT_EQ(result->as<NumericValue>(), 1.0);
}

TEST(BuiltinsTest, Expt) {
    auto result = Builtins::_expt({std::make_shared<NumericValue>(2.0), std::make_shared<NumericValue>(3.0)});
    ASSERT_EQ(result->as<NumericValue>(), 8.0);
    auto result2 = Builtins::_expt({std::make_shared<NumericValue>(2.0), std::make_shared<NumericValue>(-1.0)});
    ASSERT_EQ(result2->as<NumericValue>(), 0.5);
}

TEST(BuiltinsTest, Quotient) {
    auto result = Builtins::_quotient({std::make_shared<NumericValue>(5.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_EQ(result->as<NumericValue>(), 2.0);
    auto result2 = Builtins::_quotient({std::make_shared<NumericValue>(-5.0), std::make_shared<NumericValue>(3.0)});
    ASSERT_EQ(result2->as<NumericValue>(), -1.0);
}

TEST(BuiltinsTest, EqNum) {
    auto result = Builtins::_eq_num({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_eq_num({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
}

TEST(BuiltinsTest, Lt) {
    auto result = Builtins::_lt({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_lt({std::make_shared<NumericValue>(2.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
}

TEST(BuiltinsTest, Gt) {
    auto result = Builtins::_gt({std::make_shared<NumericValue>(2.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_gt({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
}

TEST(BuiltinsTest, Le) {
    auto result = Builtins::_le({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_le({std::make_shared<NumericValue>(2.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
    auto result3 = Builtins::_le({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_TRUE(*result3->as<BooleanValue>());
}

TEST(BuiltinsTest, Ge) {
    auto result = Builtins::_ge({std::make_shared<NumericValue>(2.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_ge({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(2.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
    auto result3 = Builtins::_ge({std::make_shared<NumericValue>(1.0), std::make_shared<NumericValue>(1.0)});
    ASSERT_TRUE(*result3->as<BooleanValue>());
}

TEST(BuiltinsTest, IsEven) {
    auto result = Builtins::_is_even({std::make_shared<NumericValue>(2.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_is_even({std::make_shared<NumericValue>(3.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
}

TEST(BuiltinsTest, IsOdd) {
    auto result = Builtins::_is_odd({std::make_shared<NumericValue>(3.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_is_odd({std::make_shared<NumericValue>(2.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
}

TEST(BuiltinsTest, IsZero) {
    auto result = Builtins::_is_zero({std::make_shared<NumericValue>(0.0)});
    ASSERT_TRUE(*result->as<BooleanValue>());
    auto result2 = Builtins::_is_zero({std::make_shared<NumericValue>(1.0)});
    ASSERT_FALSE(*result2->as<BooleanValue>());
}