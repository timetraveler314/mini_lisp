//
// Created by timetraveler314 on 5/18/24.
//

#ifndef MINI_LISP_NULLSTREAM_H
#define MINI_LISP_NULLSTREAM_H

#include <ostream>

class NullStream : public std::ostream {
public:
    NullStream() : std::ostream(nullptr) {}
    NullStream(const NullStream &) : std::ostream(nullptr) {}
};

template<typename T>
const NullStream &operator<<(NullStream &&os, const T &value) {
    return os;
}

#endif //MINI_LISP_NULLSTREAM_H
