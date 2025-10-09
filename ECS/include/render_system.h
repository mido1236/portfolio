#ifndef RENDERABLE_H
#define RENDERABLE_H
#include <chrono>
#include <SDL3_ttf/SDL_ttf.h>

#include "ecs.h"

void enableANSI();

void renderSystem(SDL_Renderer *renderer, ECS &ecs, TTF_Font *font, int score, int health);

constexpr int FPS = 60;
const std::chrono::milliseconds frameDuration(1000 / FPS);
constexpr float HUD_HEIGHT = 40.0f; // pixels reserved for text

#endif //RENDERABLE_H
