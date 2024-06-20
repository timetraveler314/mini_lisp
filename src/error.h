#ifndef ERROR_H
#define ERROR_H

#include <stdexcept>

class EvalEnv;

class SyntaxError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class EOFError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class LispError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

class LispErrorWithEnv : public LispError {
    std::shared_ptr<EvalEnv> env;
public:
    LispErrorWithEnv(const std::string& what, std::shared_ptr<EvalEnv> env) : LispError(what), env(std::move(env)) {}

    std::shared_ptr<EvalEnv> getEnv();
};

inline std::shared_ptr<EvalEnv> LispErrorWithEnv::getEnv() {
    return env;
}

#endif
