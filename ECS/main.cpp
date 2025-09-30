#include <SDL3/SDL.h>
#include <iostream>
#include <thread>

#include "ai.h"
#include "attack_system.h"
#include "cleanup.h"
#include "collision.h"
#include "components.h"
#include "ecs.h"
#include "Input.h"
#include "movement.h"
#include "player.h"
#include "player_movement.h"
#include "projectile_system.h"
#include "render_system.h"
#include "spawner_system.h"

using namespace std;

const int WIDTH = 20;
const int HEIGHT = 20;

int main() {
    Game::setupSignalHandlers();

    // SDL init
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL_Init failed: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("ECS SDL Demo", 800, 600, SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    ECS ecs;

    // Create entities
    const Entity e1 = ecs.createEntity();
    ecs.addComponent(e1, Position{2.0f, 5.0f});
    ecs.addComponent(e1, Velocity{0.0f, 0.0f});
    ecs.addComponent(e1, Player{});
    ecs.addComponent(e1, Renderable{'@'});
    ecs.addComponent(e1, Attack{10});
    ecs.addComponent(e1, Health{50, 50});

    const Entity e2 = ecs.createEntity();
    ecs.addComponent(e2, Position{10.0f, 5.0f});

    const Entity spawnerEntity = ecs.createEntity();
    Spawner sp{60, 60, 3};
    sp.types.push_back({'E', 1.0f, 5}); // slow, tanky enemy
    sp.types.push_back({'F', 2.5f, 2}); // fast, fragile enemy
    ecs.addComponent<Spawner>(spawnerEntity, sp);

    int frame = 0;
    auto lastTime = chrono::steady_clock::now();
    while (true) {
        if (collisionSystem(ecs)) break;

        auto frameStart = std::chrono::steady_clock::now();

        // clearScreen();
        Input input = inputSystem();
        if (input.quit) break;
        spawnerSystem(ecs, spawnerEntity, WIDTH);
        projectileSystem(ecs, 1);
        playerMovementSystem(ecs, input);
        attackSystem(ecs, frame, input);
        aiSystem(ecs);

        auto now = std::chrono::steady_clock::now();
        auto elapsed = now - lastTime;
        lastTime = now;
        chrono::duration<float> seconds(elapsed);

        moveSystem(ecs, seconds.count());
        renderSystem(renderer, ecs);

        frame++;

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    Game::shutdown();
    return 0;
}
