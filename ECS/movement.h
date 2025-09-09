#ifndef ECS_MOVEMENT_H
#define ECS_MOVEMENT_H
#include "ecs.h"

inline void moveSystem(ECS &ecs, const float dt) {
    for (const auto &e: ecs.queryEntities<Position, Velocity>()) {
        auto *pos = ecs.getComponent<Position>(e);
        const auto *vel = ecs.getComponent<Velocity>(e);

        pos->x += vel->dx * dt;
        pos->y += vel->dy * dt;
    }
}
#endif //ECS_MOVEMENT_H
