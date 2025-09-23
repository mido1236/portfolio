#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <chrono>

#include "ecs.h"

void clearScreen();

void enableANSI();

void renderSystem(ECS &ecs, int width, int height);

constexpr int FPS = 60;
const std::chrono::milliseconds frameDuration(1000 / FPS);

#endif //RENDERABLE_H
