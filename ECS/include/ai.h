#ifndef AI_H
#define AI_H
#include <cmath>
#include "ecs.h"

inline void aiSystem(ECS &ecs) {
    const auto players = ecs.queryEntities<Player>();

    if (players.empty()) return;

    const auto player = players.front();
    const auto *playerPos = ecs.getComponent<Position>(player);

    for (const auto &e: ecs.queryEntities<Position, AI>()) {
        const auto *pos = ecs.getComponentArray<Position>()->get(e);
        auto *vel = ecs.ensureComponent(e, Velocity({0.0f, 0.0f}));
        auto *ai = ecs.getComponentArray<AI>()->get(e);

        ai->targetX = playerPos->x;
        ai->targetY = playerPos->y;

        const float dx = ai->targetX - pos->x;
        const float dy = ai->targetY - pos->y;
        float dist = sqrtf(dx * dx + dy * dy);

        if (dist < 0.001) {
            vel->dx = vel->dy = 0;
            continue;
        }

        const float speed = min(ai->maxSpeed, dist);
        vel->dx = dx / dist * speed;
        vel->dy = dy / dist * speed;
    }
}

#endif //AI_H