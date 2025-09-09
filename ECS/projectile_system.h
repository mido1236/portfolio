//
// Created by ahmed on 2025-09-08.
//

#ifndef ECS_PROJECTILE_SYSTEM_H
#define ECS_PROJECTILE_SYSTEM_H
#include "components.h"
#include "ecs.h"

inline void projectileSystem(ECS &ecs, const int dt) {
    vector<Entity> toDestroy;

    for (auto &p: ecs.queryEntities<Projectile, Position, Velocity>()) {
        const auto *pos = ecs.getComponent<Position>(p);
        auto *proj = ecs.getComponent<Projectile>(p);

        for (auto &enemy: ecs.queryEntities<Enemy, Position, Health>()) {
            const auto *epos = ecs.getComponent<Position>(enemy);
            const auto health = ecs.getComponent<Health>(enemy);

            const float dx = epos->x - pos->x;
            const float dy = epos->y - pos->y;

            if (dx * dx + dy * dy < 1.0f) {
                health->current -= proj->damage;
                if (health->current <= 0) toDestroy.push_back(enemy);
            }
        }

        if ((proj->lifetime -= dt) <= 0) {
            toDestroy.push_back(p);
        }
    }

    for (const auto &e: toDestroy) {
        ecs.destroyEntity(e);
    }
}

#endif //ECS_PROJECTILE_SYSTEM_H
