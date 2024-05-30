//
// Created by timetraveler314 on 5/18/24.
//

#include <memory>
#include <iostream>
#include "repl.h"
#include "../tokenizer.h"
#include "../parser.h"
#include "../version.h"

void startRepl(std::istream& in, std::ostream& out, const std::shared_ptr<EvalEnv>& env, bool interactive) {
    out << std::format("Mini-Lisp {} ({}, {}, {}) [{} {}] on {}\n"
                 "Type \"help\" for more information.\n",
                       PROJECT_VERSION, PROJECT_GIT_BRANCH,
                       PROJECT_COMPILE_DATE, PROJECT_COMPILE_TIME,
                       PROJECT_CXX_COMPILER, PROJECT_CXX_COMPILER_VERSION,
                       CMAKE_HOST_SYSTEM_NAME
                       );

    while (true) {
        try {
            Tokenizer tokenizer;
            Parser parser(tokenizer);
            auto valueTask = parser.parse();

            out << ">>> ";
            std::string line;
            std::getline(in, line);
            if (in.eof()) return;

            if (interactive) {
                if (line == "exit") return;
                if (line == "help") {
                    out << "Available commands:\n"
                        << "  exit: Exit the REPL\n"
                        << "  help: Show this help message\n"
                        << "  clear: Clear the screen\n"
                        << "  reset: Reset the environment\n";
                    continue;
                }
                if (line == "clear") {
                    out << "\033[2J\033[1;1H";
                    continue;
                }
                if (line == "reset") {
                    env->reset();
                    out << "Environment reset.\n";
                    continue;
                }
            }

            tokenizer.feed(line);
            while (!valueTask.ready()) {
                out << "... ";
                std::getline(in, line);
                if (in.eof()) {
                    in.clear();
                    throw EOFError("Unexpected EOF");
                }
                tokenizer.feed(line);
            }
            auto result = env->eval(std::move(valueTask.get_result().value()));
            out << result->toString() << std::endl;
        } catch (std::runtime_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}