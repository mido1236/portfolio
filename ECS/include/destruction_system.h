//
// Created by ahmed on 2025-10-07.
//

#ifndef ECS_DESTRUCTION_SYSTEM_H
#define ECS_DESTRUCTION_SYSTEM_H
#include "ecs.h"

inline void destructionSystem(ECS &ecs, vector<Entity> &toDestroy) {
    for (const auto &e: toDestroy) {
        ecs.destroyEntity(e);
    }
    toDestroy.clear();
}

#endif //ECS_DESTRUCTION_SYSTEM_H