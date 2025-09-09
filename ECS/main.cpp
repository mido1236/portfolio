#include <iostream>
#include <thread>

#include "ai.h"
#include "attack_system.h"
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

int main() {
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

    const Entity spawner = ecs.createEntity();
    ecs.addComponent<Spawner>(spawner, {120, 60});

    int frame = 0;
    while (true) {
        if (collisionSystem(ecs)) break;

        auto frameStart = std::chrono::steady_clock::now();

        clearScreen();
        Input input = inputSystem();
        spawnerSystem(ecs, spawner);
        projectileSystem(ecs, 1);
        playerMovementSystem(ecs, input);
        attackSystem(ecs, frame, input);
        aiSystem(ecs);
        moveSystem(ecs, 1);
        cout << "Frame " << frame << endl;
        renderSystem(ecs, 20, 20);

        frame++;

        auto frameEnd = std::chrono::steady_clock::now();
        auto elapsed = frameEnd - frameStart;

        if (elapsed < frameDuration) {
            std::this_thread::sleep_for(frameDuration - elapsed);
        }
    }


    return 0;
}
