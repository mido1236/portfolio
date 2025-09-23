//
// Created by ahmed on 2025-08-28.
//

#ifndef ECS_ATTACK_SYSTEM_H
#define ECS_ATTACK_SYSTEM_H
#include <conio.h>

#include "ecs.h"
#include "Input.h"

inline void attackSystem(ECS &ecs, const int frame, Input &input) {
    for (const auto &e: ecs.queryEntities<Player, Attack, Position>()) {
        if (input.attack) {
            auto *atk = ecs.getComponent<Attack>(e);
            const auto *pos = ecs.getComponent<Position>(e);

            if (frame - atk->lastAttackTime < atk->cooldown) continue;
            atk->lastAttackTime = frame;

            auto projectile = ecs.createEntity();
            ecs.addComponent<Projectile>(projectile, {atk->damage, 10});
            ecs.addComponent<Position>(projectile, {pos->x, pos->y});
            ecs.addComponent<Velocity>(projectile, {0, -30});
            ecs.addComponent<Renderable>(projectile, {'*'});
        }
    }
}

#endif //ECS_ATTACK_SYSTEM_H
