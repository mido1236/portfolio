#ifndef ECS_COMPONENTS_H
#define ECS_COMPONENTS_H
#include <bitset>

#include "entity.h"

constexpr size_t MAX_COMPONENTS = 64;
using Signature = std::bitset<MAX_COMPONENTS>;

using ComponentTypeID = std::size_t;
inline ComponentTypeID nextComponentTypeID = 0;

struct Health {
    int current;
    int max;
};

struct Score {
    int current;
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
    SDL_Texture *texture;
    int damage;
    float fireRate;
    float cooldown;
};

struct Projectile {
    int damage;
    int lifetime; // how many frames it lasts
    Entity player;
};

struct Sprite {
    SDL_Texture *texture;
    SDL_FRect srcRect;
    SDL_FRect dstRect;
};

#endif //ECS_COMPONENTS_H