//
// Created by ahmed on 2025-10-07.
//

#ifndef ECS_EVENT_SYSTEM_H
#define ECS_EVENT_SYSTEM_H
#include "ecs.h"

enum EventType {
    EnemyDestroyed,
    PlayerDamaged
};

struct Event {
    EventType type;
    Entity source;
    Entity target;
};

inline void eventSystem(ECS &ecs, vector<Event> &events) {
    for (const auto &[type, source, target]: events) {
        switch (type) {
            case EnemyDestroyed: {
                const auto score = ecs.getComponent<Score>(source);
                score->current += 10;
                break;
            }
            case PlayerDamaged: {
                const auto health = ecs.getComponent<Health>(target);
                health->current -= 1;
                break;
            }
        }
    }
    events.clear();
};
#endif //ECS_EVENT_SYSTEM_H
