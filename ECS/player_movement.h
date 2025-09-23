//
// Created by ahmed on 2025-09-02.
//

#ifndef ECS_PLAYER_MOVEMENT_H
#define ECS_PLAYER_MOVEMENT_H
#include "ecs.h"
#include "Input.h"

inline void playerMovementSystem(ECS &ecs, const Input &input) {
    for (const auto &e: ecs.queryEntities<Player>()) {
        auto *vel = ecs.ensureComponent<Velocity>(e, {0, 0});

        vel->dy = vel->dx = 0;
        if (input.up) vel->dy = -10;
        if (input.down) vel->dy = 10;
        if (input.left) vel->dx = -10;
        if (input.right) vel->dx = 10;
    }
}

#endif //ECS_PLAYER_MOVEMENT_H
