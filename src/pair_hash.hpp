#pragma once

#include <cstddef>
#include <functional>

struct PairHash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        const size_t t1Hash = std::hash<T1>{}(pair.first);
        const size_t t2Hash = std::hash<T2>{}(pair.second);

        return t1Hash ^ t2Hash;
    }
};
