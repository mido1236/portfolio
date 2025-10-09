#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <windows.h>
#include "../include/render_system.h"

#include <vector>

using namespace std;

constexpr int CELL_SIZE = 20; // each entity = 20x20 pixels

char glyphForEntity(ECS &ecs, const Entity &e) {
    const auto *renderable = ecs.getComponent<Renderable>(e);
    if (renderable != nullptr) return renderable->glyph;
    if (ecs.getComponent<Player>(e)) return '@';
    if (ecs.getComponent<AI>(e)) return 'E';
    if (ecs.getComponent<Projectile>(e)) return '*';
    return '?';
}

inline SDL_Texture *renderText(SDL_Renderer *renderer, TTF_Font *font,
                               const std::string &message, const SDL_Color color) {
    SDL_Surface *surf = TTF_RenderText_Solid(font, message.c_str(), 0, color);
    if (!surf) return nullptr;

    SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
    return tex;
}

inline void renderHUD(SDL_Renderer *renderer, TTF_Font *font, const int score, const int health) {
    const SDL_Color white{255, 255, 255, 255};
    const std::string hudText = "Score: " + std::to_string(score) + "  HP: " + std::to_string(health);

    if (SDL_Texture *textTex = renderText(renderer, font, hudText, white)) {
        float w, h;
        SDL_GetTextureSize(textTex, &w, &h);
        const SDL_FRect dst{10.0f, 10.0f, w, h};
        SDL_RenderTexture(renderer, textTex, nullptr, &dst);
        SDL_DestroyTexture(textTex);
    }
}


void renderSystem(SDL_Renderer *renderer, ECS &ecs, TTF_Font *font, const int score, const int health) {
    // Clear screen black
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Draw all entities with Position plus Renderable
    for (const auto e: ecs.queryEntities<Position, Sprite>()) {
        const auto *pos = ecs.getComponent<Position>(e);
        auto *sprite = ecs.getComponent<Sprite>(e);

        sprite->dstRect.x = pos->x;
        sprite->dstRect.y = pos->y + HUD_HEIGHT;

        SDL_RenderTexture(renderer, sprite->texture, &sprite->srcRect, &sprite->dstRect);
    }

    renderHUD(renderer, font, score, health);

    SDL_RenderPresent(renderer);
}

void enableANSI() {
    const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
