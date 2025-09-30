#include <SDL3/SDL.h>
#include <windows.h>
#include "render_system.h"

#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

constexpr int CELL_SIZE = 20;  // each entity = 20x20 pixels

char glyphForEntity(ECS &ecs, const Entity &e) {
    auto *renderable = ecs.getComponent<Renderable>(e);
    if (renderable != nullptr) return renderable->glyph;
    if (ecs.getComponent<Player>(e)) return '@';
    if (ecs.getComponent<AI>(e)) return 'E';
    if (ecs.getComponent<Projectile>(e)) return '*';
    return '?';
}

void renderSystem(SDL_Renderer* renderer, ECS& ecs) {
    // Clear screen black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw all entities with Position + Renderable
    for (auto e : ecs.queryEntities<Position, Renderable>()) {
        auto* pos = ecs.getComponent<Position>(e);
        auto* rend = ecs.getComponent<Renderable>(e);

        SDL_FRect rect;
        rect.x = static_cast<int>(pos->x * CELL_SIZE);
        rect.y = static_cast<int>(pos->y * CELL_SIZE);
        rect.w = CELL_SIZE;
        rect.h = CELL_SIZE;

        // Pick a color by glyph (just as an example)
        switch (rend->glyph) {
            case '@': SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); break; // player = green
            case 'E': SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break; // enemy = red
            case '*': SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); break; // projectile = yellow
            default:  SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255); break;
        }

        SDL_RenderFillRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
}

void clearScreen() {
    // std::cout << "\033[2J\033[H";
    // std::cout << "\033[H";
}

void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
