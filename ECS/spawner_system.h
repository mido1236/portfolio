//
// Created by ahmed on 2025-09-08.
//

#ifndef ECS_SPAWNER_SYSTEM_H
#define ECS_SPAWNER_SYSTEM_H
#include <random>

#include "custom_random.h"
#include "ecs.h"

struct EnemyType {
    char glyph; // how it looks
    float maxSpeed; // movement speed
    int health; // starting health
};

struct Spawner {
    int interval;
    int timer;
    int enemiesPerWave;
    vector<EnemyType> types;
};

inline void spawnerSystem(ECS &ecs, const Entity &se, const int width) {
    auto *spawner = ecs.getComponent<Spawner>(se);
    if (!spawner) return;

    if (--spawner->timer > 0) return;

    spawner->timer = spawner->interval;
    const Entity entity = ecs.createEntity();

    for (int i = 0; i < spawner->enemiesPerWave; i++) {
        int rand_int = randInt(0, static_cast<int>(spawner->types.size()) - 1);
        auto [glyph, maxSpeed, health] = spawner->types[rand_int];
        ecs.addComponent<Enemy>(entity, {});
        ecs.addComponent<Position>(entity, {static_cast<float>(randInt(0, width)), 0});
        ecs.addComponent<Velocity>(entity, {0, 0});
        ecs.addComponent<Renderable>(entity, {glyph});
        ecs.addComponent<Health>(entity, {health, health});
        ecs.addComponent<AI>(entity, {0, 0, maxSpeed});
    }
}
#endif //ECS_SPAWNER_SYSTEM_H
