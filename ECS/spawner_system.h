//
// Created by ahmed on 2025-09-08.
//

#ifndef ECS_SPAWNER_SYSTEM_H
#define ECS_SPAWNER_SYSTEM_H
#include "ecs.h"

inline void spawnerSystem(ECS &ecs, const Entity &se) {
    auto *s = ecs.getComponent<Spawner>(se);
    if (!s) return;

    if (--s->timer > 0) return;

    s->timer = s->interval;
    const Entity entity = ecs.createEntity();

    ecs.addComponent<Enemy>(entity, {});
    ecs.addComponent<Position>(entity, {0, 0});
    ecs.addComponent<Velocity>(entity, {0, 0});
    ecs.addComponent<Renderable>(entity, {'E'});
    ecs.addComponent<Health>(entity, {10});
    ecs.addComponent<AI>(entity, {0, 0, 0.01f});
}
#endif //ECS_SPAWNER_SYSTEM_H
