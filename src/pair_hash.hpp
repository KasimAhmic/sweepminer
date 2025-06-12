#pragma once

struct PairHash {
    std::size_t operator()(const std::pair<int32_t, int32_t>& pair) const {
        return std::hash<int32_t>()(pair.first) ^ std::hash<int32_t>()(pair.second) << 1;
    }
};
