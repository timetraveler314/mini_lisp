#include <iostream>
#include <fstream>
#include <cxxopts.hpp>
#include <filesystem>

#include "modes/repl.h"
#include "utils/nullstream.h"
#include "eval_env.h"

int main(int argc, char* argv[]) {
    cxxopts::Options options("MiniLisp", "A simple lisp interpreter of the course"
                                         "\"Practice of Software Design\", 2024 Spring");
    options.add_options()
            ("i,input", "Input file", cxxopts::value<std::string>())
            ("s,save", "Save file", cxxopts::value<std::string>())
            ("r,repl", "Start repl mode (after file mode)")
            ;

    try {
        auto result = options.parse(argc, argv);

        std::shared_ptr<EvalEnv> env = EvalEnv::createGlobal();
        std::shared_ptr<std::ofstream> save = std::make_shared<NullFileStream>();

        if (result.count("save")) {
            auto fileName = result["save"].as<std::string>();
            save = std::make_shared<std::ofstream>(fileName);
            if (!save->is_open()) {
                throw std::runtime_error("Failed to open file: " + fileName);
            }
        }

        if (result.count("input")) {
            auto fileName = result["input"].as<std::string>();
            if (!std::filesystem::exists(fileName)) {
                throw std::runtime_error("File not found: " + fileName);
            }
            auto file = std::ifstream(fileName);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file: " + fileName);
            }

            auto null = NullStream();
            startRepl(file, null, save, env);

            if (result.count("repl")) {
                std::cout << std::endl;
                std::cout << "Evaluated file " << fileName << ". ";
                std::cout << "Entering REPL mode..." << std::endl;
                startRepl(std::cin, std::cout, save, env, true);
            }
            return 0;
        }

        startRepl(std::cin, std::cout, save, env, true);
        return 0;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}