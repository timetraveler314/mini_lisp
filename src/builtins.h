//
// Created by timetraveler314 on 5/4/24.
//

#ifndef MINI_LISP_BUILTINS_H
#define MINI_LISP_BUILTINS_H

// Built-in procedures

#include "value.h"
#include "error.h"

namespace Builtins {
    extern const std::unordered_map<std::string, ValuePtr> builtinMap;

    // (+ n1 n2 ... nk)
    ValuePtr add(const std::vector<ValuePtr>& params);
}

#endif //MINI_LISP_BUILTINS_H
