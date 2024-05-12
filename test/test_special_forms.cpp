//
// Created by timetraveler314 on 5/12/24.
//
#include <gtest/gtest.h>
#include "../src/eval_env.h"
#include "../src/tokenizer.h"
#include "../src/parser.h"

class SpecialFormsTest : public ::testing::Test {
protected:
    void SetUp() override {
        env = EvalEnv();
    }

    EvalEnv env;

    std::string eval(const std::string& input) {
        auto tokens = Tokenizer::tokenize(input);
        Parser parser(std::move(tokens));
        auto value = parser.parse();
        auto result = env.eval(std::move(value));
        return result->toString();
    }
};

TEST_F(SpecialFormsTest, Define) {
    EXPECT_EQ(eval("(define define-1 1)"), "()");
    EXPECT_EQ(eval("define-1"), "1");
}

TEST_F(SpecialFormsTest, Quote) {
    EXPECT_EQ(eval("(quote ())"), "()");
    EXPECT_EQ(eval("(quote 1)"), "1");
}

TEST_F(SpecialFormsTest, If) {
    EXPECT_EQ(eval("(if #t 1 2)"), "1");
    EXPECT_EQ(eval("(if #f 1 2)"), "2");

    // Test short-circuiting
    EXPECT_EQ(eval("(if #t 1 ())"), "1"); // Won't evaluate (), so it won't throw an error
}

TEST_F(SpecialFormsTest, And) {
    EXPECT_EQ(eval("(and)"), "#t");
    EXPECT_EQ(eval("(and #t #f)"), "#f");
    EXPECT_EQ(eval("(and #t #t)"), "#t");
    EXPECT_EQ(eval("(and 1 2 3)"), "3");

    // Test short-circuiting
    testing::internal::CaptureStdout();
    EXPECT_EQ(eval("(and (print 1) (print 2) #f (print \"error\"))"), "#f");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n"); // Should not print "error"
}

TEST_F(SpecialFormsTest, Or) {
    EXPECT_EQ(eval("(or)"), "#f");
    EXPECT_EQ(eval("(or #t #f)"), "#t");
    EXPECT_EQ(eval("(or #f #f)"), "#f");
    EXPECT_EQ(eval("(or 1 2 3)"), "1");

    // Test short-circuiting
    testing::internal::CaptureStdout();
    EXPECT_EQ(eval("(or #f #f (print 1) (print \"error\"))"), "()");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1\n"); // Should not print "error"
}