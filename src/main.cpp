#include <iostream>
#include <string>

#include "./tokenizer.h"
#include "parser.h"

#include "rjsj_test.hpp"
#include "eval_env.h"

struct TestCtx {
    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
    TestCtx() : env(EvalEnv::createGlobal()) {};

    std::string eval(const std::string& input) {
        Tokenizer tokenizer;
        Parser parser(tokenizer);
        auto valueTask = parser.parse();
        tokenizer.feed(input);
        auto result = env->eval(valueTask.get_result().value());
        return result->toString();
    }
};

int main() {
    // RJSJ_TEST(TestCtx, Lv2, Lv3, Lv4, Lv5, Lv5Extra, Lv6, Lv7, Lv7Lib, Sicp);
    std::cout << "Mini-Lisp 0.0.1 (coroutine, May 18 2024, 11:07:11) [GCC 14.1.1 20240507] on linux\n"
                 "Type \"help\", \"copyright\", \"credits\" or \"license\" for no information.\n";

    std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();

    while (true) {
        try {
            Tokenizer tokenizer;
            Parser parser(tokenizer);
            auto valueTask = parser.parse();

            std::cout << ">>> " ;
            std::string line;
            std::getline(std::cin, line);
            if (std::cin.eof()) std::exit(0);
            tokenizer.feed(line);
            while (!valueTask.ready()) {
                std::cout << "... ";
                std::getline(std::cin, line);
                if (std::cin.eof()) {
                    std::cin.clear();
                    throw EOFError("Unexpected EOF");
                }
                tokenizer.feed(line);
            }
            auto result = env->eval(std::move(valueTask.get_result().value()));
            std::cout << result->toString() << std::endl;
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}
