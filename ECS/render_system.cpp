#include <windows.h>
#include "render_system.h"

#include <iostream>
#include <ostream>
#include <vector>

using namespace std;

char glyphForEntity(ECS &ecs, const Entity &e) {
    if (ecs.getComponent<Player>(e)) return '@';
    if (ecs.getComponent<AI>(e)) return 'E';
    if (ecs.getComponent<Projectile>(e)) return '*';
    return '?';
}

void renderSystem(ECS &ecs, const int width, const int height) {
    vector grid(height, string(width, '.'));

    for (const auto &e: ecs.queryEntities<Position>()) {
        const auto *pos = ecs.getComponent<Position>(e);

        if (pos->x >= 0 && pos->y >= 0 && pos->x < width && pos->y < height) {
            grid[pos->y][pos->x] = glyphForEntity(ecs, e);
        }
    }

    for (auto l: grid) {
        cout << l << endl;
    }
    cout << "=====" << endl;
}

void clearScreen() {
    // std::cout << "\033[2J\033[H";
    std::cout << "\033[H";
}

void enableANSI() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
