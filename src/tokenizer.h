#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <deque>
#include <string>
#include <coroutine>
#include <queue>

#include "./token.h"

class Tokenizer {
private:
    int pos;
    std::string input;
    std::deque<TokenPtr> tokens;
    std::queue<std::coroutine_handle<>> waiting;

    TokenPtr tokenizeNext(int& pos);

public:
    inline void reset() {
        pos = 0;
        tokens.clear();
        input.clear();
        while (!waiting.empty()) {
            waiting.pop();
        }
    }

    void feed(const std::string& str);

    auto awaitNextToken() {
        struct Awaiter {
            Tokenizer& tokenizer;

            explicit Awaiter(Tokenizer& tokenizer) : tokenizer(tokenizer) {}

            bool await_ready() {
                return !tokenizer.tokens.empty();
            }

            void await_suspend(std::coroutine_handle<> handle) {
                tokenizer.waiting.emplace(handle);
            }

            TokenPtr await_resume() {
                auto token = std::move(tokenizer.tokens.front());
                tokenizer.tokens.pop_front();
                return token;
            }
        };
        return Awaiter(*this);
    }

    auto awaitPeekNextToken() {
        struct Awaiter {
            Tokenizer& tokenizer;

            explicit Awaiter(Tokenizer& tokenizer) : tokenizer(tokenizer) {}

            bool await_ready() {
                return !tokenizer.tokens.empty();
            }

            void await_suspend(std::coroutine_handle<> handle) {
                tokenizer.waiting.emplace(handle);
            }

            TokenType await_resume() {
                return tokenizer.tokens.front()->getType();
            }
        };
        return Awaiter(*this);
    }

    Tokenizer() : pos(0), tokens(), input(), waiting() {}

    static std::deque<TokenPtr> __legacyTokenize(const std::string &input);
};

#endif