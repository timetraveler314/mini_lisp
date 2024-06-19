//
// Created by timetraveler314 on 5/18/24.
//

#include <memory>
#include <iostream>
#include "repl.h"
#include "../tokenizer.h"
#include "../parser.h"
#include "../version.h"

void getInput(std::istream &in, std::ostream &out, std::string &program, std::string &line) {
    std::getline(in, line);
    line += "\n"; // Compensate for the trailing newline
    program += line;
}

void startRepl(std::istream& in, std::ostream& out, const std::shared_ptr<std::ostream>& save, const std::shared_ptr<EvalEnv>& env, bool interactive) {
    // Show banner
    out << "MINI-LISP : Minilisp Is Not Implemented Like In Standard Practice" << std::endl;

    out << std::format("Mini-Lisp {} ({}, {}, {}) [{} {}] on {}\n"
                 "Type \"help\" for more information.\n",
                       PROJECT_VERSION, PROJECT_GIT_BRANCH,
                       PROJECT_COMPILE_DATE, PROJECT_COMPILE_TIME,
                       PROJECT_CXX_COMPILER, PROJECT_CXX_COMPILER_VERSION,
                       CMAKE_HOST_SYSTEM_NAME
                       );
    std::vector<std::string> history;
    std::deque<std::string> buffer;
    int lineCount = 1;

    while (true) {
        try {
            Tokenizer tokenizer;
            if (!interactive) tokenizer.setLineCount(lineCount);
            Parser parser(tokenizer);
            auto valueTask = parser.parse();

            std::string program;

            std::string line;
            out << ">>> ";
            getInput(in, out, program, line);
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
                if (line == "save") {
                    std::cout << "Saving to file...\n";
                    while (!buffer.empty()) {
                        *save << buffer.front();
                        buffer.pop_front();
                    }
                    save->flush();
                    continue;
                }
            }

            tokenizer.feed(line);
            while (!valueTask.ready()) {
                out << "... ";
                getInput(in, out, program, line);
                if (in.eof()) {
                    in.clear();
                    throw EOFError("Unexpected EOF");
                }
                tokenizer.feed(line);
            }
            auto result = env->eval(std::move(valueTask.get_result().value()));
            out << result->toString() << std::endl;

            lineCount = tokenizer.getLineCount();

            history.push_back(program);
            buffer.push_back(program);
        } catch (LispErrorWithEnv& e) {
            auto errorEnv = e.getEnv();
            std::cerr << "Traceback (most recent call last):\n";
            std::cerr << errorEnv->generateStackTrace(5);
            errorEnv->clearStack();
            std::cerr << "Error: " << e.what() << std::endl;

            if (!interactive) std::exit(1);
        } catch (std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            if (!interactive) std::exit(1);
        }
    }
}
