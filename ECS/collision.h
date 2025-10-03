//
// Created by ahmed on 2025-08-25.
//

#ifndef ECS_COLLISION_H
#define ECS_COLLISION_H
#include "ecs.h"

inline bool checkCollision(const Position &a, const Position &b) {
    // Simple integer-based grid collision
    return static_cast<int>(a.x) == static_cast<int>(b.x)
           && static_cast<int>(a.y) == static_cast<int>(b.y);
}

inline bool collisionSystem(ECS &ecs) {
    const auto players = ecs.queryEntities<Player, Health>();
    if (players.empty()) return false;

    const auto player = players.front();
    const auto *playerPos = ecs.getComponent<Position>(player);
    auto *playerHealth = ecs.getComponent<Health>(player);

    for (const auto &e: ecs.queryEntities<AI, Position>()) {
        const auto *aiPos = ecs.getComponent<Position>(e);

        if (checkCollision(*playerPos, *aiPos)) {
            playerHealth->current--;
            if (playerHealth->current <= 0) {
                cout << "DEAD" << endl;
                return true;
            }
        }
    }

    return false;
};

#endif //ECS_COLLISION_H
