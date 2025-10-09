//
// Created by ahmed on 2025-09-22.
//

#ifndef ECS_CUSTOM_RANDOM_H
#define ECS_CUSTOM_RANDOM_H
#include <random>

// global or static engine, seeded once
inline std::mt19937 &rng() {
    static std::mt19937 gen{std::random_device{}()};
    return gen;
}

inline int randInt(int min, int max) {
    std::uniform_int_distribution<> dist(min, max);
    return dist(rng());
}
#endif //ECS_CUSTOM_RANDOM_H
