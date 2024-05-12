//
// Created by timetraveler314 on 5/12/24.
//

#ifndef MINI_LISP_UTILS_H
#define MINI_LISP_UTILS_H

#include <vector>
#include <string>
#include "value.h"

namespace Utils {
    void checkParams(const std::vector<ValuePtr>& params, size_t exact, const std::string& name);
    void checkParams(const std::vector<ValuePtr>& params, size_t min, size_t max, const std::string& name);
}

#endif //MINI_LISP_UTILS_H
