#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <chrono>

#include "ecs.h"

void enableANSI();

void renderSystem(SDL_Renderer* renderer, ECS& ecs);

constexpr int FPS = 60;
const std::chrono::milliseconds frameDuration(1000 / FPS);

#endif //RENDERABLE_H
