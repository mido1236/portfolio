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

    enableANSI();
    std::cout << "\033[2J\033[H";

    ECS ecs;

    // Create entities
    const Entity e1 = ecs.createEntity();
    ecs.addComponent(e1, Position{2.0f, 5.0f});
    ecs.addComponent(e1, Velocity{0.0f, 0.0f});
    ecs.addComponent(e1, Player{});
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

        clearScreen();
        Input input = inputSystem();
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
        cout << "Frame " << frame << "              " << endl;
        renderSystem(ecs, WIDTH, HEIGHT);

        frame++;

        auto frameEnd = std::chrono::steady_clock::now();
        elapsed = frameEnd - frameStart;

        if (elapsed < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsed);
        }
    }

    Game::shutdown();
    return 0;
}
