//
// Created by timetraveler314 on 5/12/24.
//

#include "utils.h"
#include "error.h"

namespace Utils {
    void checkParams(const std::vector<ValuePtr>& params, size_t exact, const std::string& name) {
        if (params.size() != exact) {
            throw LispError(name + ": expected " + std::to_string(exact) + " arguments, but got " + std::to_string(params.size()));
        }
    }

    void checkParams(const std::vector<ValuePtr>& params, size_t min, size_t max, const std::string& name) {
        if (params.size() < min || params.size() > max) {
            throw LispError(name + ": expected " + std::to_string(min) + " to " + std::to_string(max) + " arguments, but got " + std::to_string(params.size()));
        }
    }
}