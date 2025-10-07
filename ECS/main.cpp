#include <SDL3/SDL.h>
#include <iostream>
#include <thread>

#include "ai.h"
#include "attack_system.h"
#include "cleanup.h"
#include "collision.h"
#include "components.h"
#include "ecs.h"
#include "image.h"
#include "Input.h"
#include "movement.h"
#include "player.h"
#include "player_movement.h"
#include "projectile_system.h"
#include "render_system.h"
#include "spawner_system.h"
#include "SDL3_ttf/SDL_ttf.h"

using namespace std;

const int WIDTH = 200;
const int HEIGHT = 200;

int main() {
    Game::setupSignalHandlers();

    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    // if (IMG_Init(IMG_INIT_PNG) == 0) {
    //     SDL_Log("Failed to init SDL_image: %s", IMG_GetError());
    //     return 1;
    // }

    if (!TTF_Init()) {
        SDL_Log("Failed to initialize SDL_ttf: %s", SDL_GetError());
        return 1;
    }

    TTF_Font *font = TTF_OpenFont("assets/PressStart2P.ttf", 16);
    if (!font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("ECS SDL Demo", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    // Load textures
    SDL_Texture *spritesTex = loadTexture(renderer, "assets/sprites.png");

    ECS ecs;

    // Create entities
    const Entity playerEntity = ecs.createEntity();
    ecs.addComponent(playerEntity, Position{20.0f, 50.0f});
    ecs.addComponent(playerEntity, Velocity{0.0f, 0.0f});
    ecs.addComponent(playerEntity, Player{});
    ecs.addComponent(playerEntity, Renderable{'@'});
    ecs.addComponent(playerEntity, Attack{spritesTex, 10, 0.15f, 0});
    ecs.addComponent(playerEntity, Health{50, 50});
    ecs.addComponent(playerEntity, Score{0});
    ecs.addComponent(playerEntity, Sprite{spritesTex, {0, 0, 350, 450}, {0, 0, 32, 32}});

    const Entity e2 = ecs.createEntity();
    ecs.addComponent(e2, Position{10.0f, 5.0f});

    const Entity spawnerEntity = ecs.createEntity();
    Spawner sp{60, 20, 3};
    sp.types.push_back({'E', 20.0f, 5, Sprite{spritesTex, {445, 0, 360, 450}, {0, 0, 32, 32}}}); // slow, tanky enemy
    sp.types.push_back({'F', 50.0f, 2, Sprite{spritesTex, {445, 0, 360, 450}, {0, 0, 32, 32}}}); // fast, fragile enemy
    ecs.addComponent<Spawner>(spawnerEntity, sp);

    int frame = 0;
    auto lastTime = chrono::steady_clock::now();
    while (true) {
        if (collisionSystem(ecs)) break;

        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - lastTime;
        lastTime = now;
        chrono::duration<float> seconds(elapsed);

        Input input = inputSystem();
        if (input.quit) break;
        spawnerSystem(ecs, spawnerEntity, WIDTH);
        projectileSystem(ecs, 1);
        playerMovementSystem(ecs, input);
        attackSystem(ecs, seconds.count(), input);
        aiSystem(ecs);
        moveSystem(ecs, seconds.count());
        auto health = ecs.getComponent<Health>(playerEntity);
        auto score = ecs.getComponent<Score>(playerEntity);
        renderSystem(renderer, ecs, font, score->current, health->current);

        frame++;

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Game::shutdown();
    return 0;
}
