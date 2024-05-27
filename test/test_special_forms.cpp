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
        env = EvalEnv::createGlobal();
    }

    std::shared_ptr<EvalEnv> env;

    std::string eval(const std::string& input) {
        Tokenizer tokenizer;
        Parser parser(tokenizer);
        auto valueTask = parser.parse();
        tokenizer.feed(input);
        auto result = env->eval(valueTask.get_result().value());
        return result->toString();
    }
};

TEST_F(SpecialFormsTest, Define) {
    EXPECT_EQ(eval("(define define-1 1)"), "()");
    EXPECT_EQ(eval("define-1"), "1");

    // Defining lambda
    EXPECT_EQ(eval("(define (add a b) (+ a b))"), "()");
    EXPECT_EQ(eval("add"), "#<procedure>");
    EXPECT_EQ(eval("(add 1 2)"), "3");

    // Defining lambda with multiple expressions
    EXPECT_EQ(eval("(define (add a b) (print a) (print b) (+ a b))"), "()");
    testing::internal::CaptureStdout();
    EXPECT_EQ(eval("(add 1 2)"), "3");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n");

    // Extreme cases
    EXPECT_THROW(eval("(define)"), LispError);
    EXPECT_THROW(eval("(define 1)"), LispError);
    EXPECT_THROW(eval("(define (#t x))"), LispError);
    EXPECT_THROW(eval("(define (f x))"), LispError);
    EXPECT_THROW(eval("(define 1 2 3)"), LispError);

    EXPECT_THROW(eval("(define (f x 1 y) (+ x y))"), LispError);
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

    // Undefined behavior
    EXPECT_EQ(eval("(if #f 1)"), "()"); // the implementation should return ()
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

TEST_F(SpecialFormsTest, Cond) {
    EXPECT_EQ(eval("(cond)"), "()");
    EXPECT_EQ(eval("(cond (#f 1) (#f 2))"), "()");
    EXPECT_EQ(eval("(cond (\"notFalse\") (#f 2))"), "\"notFalse\"");

    testing::internal::CaptureStdout();
    EXPECT_EQ(eval("(cond (#t (print 1) (print 2) 3) (#f 4))"), "3");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n");
    testing::internal::CaptureStdout();
    EXPECT_EQ(eval("(cond (#f 0) (else (print 1) (print 2) 3))"), "3");
    EXPECT_EQ(testing::internal::GetCapturedStdout(), "1\n2\n");

    EXPECT_THROW(eval("(cond (#f 1) (else))"), LispError);
    EXPECT_THROW(eval("(cond (else 0) (#f 1))"), LispError);
    EXPECT_THROW(eval("(cond #f (#f 1))"), LispError);
}