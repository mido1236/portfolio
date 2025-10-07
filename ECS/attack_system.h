//
// Created by ahmed on 2025-08-28.
//

#ifndef ECS_ATTACK_SYSTEM_H
#define ECS_ATTACK_SYSTEM_H
#include <conio.h>

#include "ecs.h"
#include "Input.h"

inline void attackSystem(ECS &ecs, const float dt, Input &input) {
    for (const auto &e: ecs.queryEntities<Player, Attack, Position>()) {
        if (input.attack) {
            auto *atk = ecs.getComponent<Attack>(e);
            const auto *pos = ecs.getComponent<Position>(e);

            atk->cooldown -= dt;

            if (atk->cooldown > 0) return;

            atk->cooldown = atk->fireRate;
            auto projectile = ecs.createEntity();
            ecs.addComponent<Projectile>(projectile, {atk->damage, 30});
            ecs.addComponent<Position>(projectile, {pos->x + 8, pos->y});
            ecs.addComponent<Velocity>(projectile, {0, -300});
            ecs.addComponent<Renderable>(projectile, {'*'});
            ecs.addComponent<Sprite>(projectile, {atk->texture, {944, 0, 245, 450}, {0, 0, 16, 16}});
        }
    }
}

#endif //ECS_ATTACK_SYSTEM_H
