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

class NullFileStream : public std::ofstream {
public:
    NullFileStream() : std::ofstream(nullptr) {}
    NullFileStream(const NullFileStream &) : std::ofstream(nullptr) {}
};

template<typename T>
const NullFileStream &operator<<(NullFileStream &&os, const T &value) {
    return os;
}

#endif //MINI_LISP_NULLSTREAM_H
