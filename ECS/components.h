#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H
#include <bitset>

constexpr size_t MAX_COMPONENTS = 64;
using Signature = std::bitset<MAX_COMPONENTS>;

using ComponentTypeID = std::size_t;
inline ComponentTypeID nextComponentTypeID = 0;

struct Health {
    int current;
    int max;
};

struct Position {
    float x, y;
};

struct Velocity {
    float dx, dy;
};

struct AI {
    float targetX;
    float targetY;
    float maxSpeed; // maximum speed
};

struct Player {};

struct Enemy {};

struct Renderable {
    char glyph;
};

struct Attack {
    int damage;
    int cooldown;
    int lastAttackTime;
};

struct Projectile {
    int damage;
    int lifetime; // how many frames it lasts
};

struct Spawner {
    int interval;
    int timer;
};

#endif //ECS_COMPONENTS_H
