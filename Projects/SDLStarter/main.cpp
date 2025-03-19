#include <SDL3/SDL.h>
#include <SDL3/SDL_image.h>
#include <iostream>
#include <vector>

using namespace std;

struct Block {
    float x, y, w, h, speed;
};

std::vector<Block> blocks; // List of obstacles

void spawnBlock() {
    blocks.push_back({(float) (rand() % 750), 0, 50, 50, (float) (rand() % 5 + 2)});
}

void updateBlocks() {
    for (auto &block: blocks) {
        block.y += block.speed;
        if (block.y > 600) block.y = 0; // Respawn at the top
    }
}

void renderBlocks(SDL_Renderer *renderer, SDL_Texture *blockTexture) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red
    for (const auto &block: blocks) {
        SDL_FRect rect = {block.x, block.y, block.w, block.h};
        SDL_RenderTexture(renderer, blockTexture, nullptr, &rect);
    }
}

bool isColliding(const SDL_FRect &a, const SDL_FRect &b) {
    return (a.x < b.x + b.w && a.x + a.w > b.x &&
            a.y < b.y + b.h && a.y + a.h > b.y);
}

bool check_collisions(SDL_FRect player) {
    for (const auto &block: blocks) {
        if (isColliding(player, {block.x, block.y, block.w, block.h})) {
            std::cout << "Game Over!\n";
            return false;
        }
    }
    return true;
}

void move_player(int playerSpeed, SDL_FRect &player) {
    const bool *keystate = SDL_GetKeyboardState(nullptr);
    if (keystate[SDL_SCANCODE_LEFT]) player.x -= playerSpeed;
    if (keystate[SDL_SCANCODE_RIGHT]) player.x += playerSpeed;
    if (keystate[SDL_SCANCODE_UP]) player.y -= playerSpeed;
    if (keystate[SDL_SCANCODE_DOWN]) player.y += playerSpeed;

    // Keep player inside the window
    if (player.x < 0) player.x = 0;
    if (player.x > 750) player.x = 750;
    if (player.y < 0) player.y = 0;
    if (player.y > 550) player.y = 550;
}

bool handle_restart(SDL_Event event, const Uint32 startTime) {
    int score = (SDL_GetTicks() - startTime) / 1000; // Score in seconds
    std::cout << "Final Score: " << score << "\nPress R to Restart!\n";
    while (SDL_WaitEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        if (event.type == SDL_EVENT_KEY_DOWN && event.key.scancode == SDL_SCANCODE_R) {
            blocks.clear();
            return true;
        }
    }
    return false;
}

// Load texture function
SDL_Texture *loadTexture(SDL_Renderer *renderer, const char *file) {
    SDL_Texture *texture = IMG_LoadTexture(renderer, file);
    if (!texture) {
        std::cerr << "Failed to load image: " << std::endl;
    }
    return texture;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    // IMG_Init(IMG_INIT_PNG); // Enable PNG support

    SDL_Window *window = SDL_CreateWindow("Dodge the Blocks", 800, 600, SDL_WINDOW_VULKAN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, nullptr);

    SDL_Texture *playerTexture = loadTexture(renderer, "player.png");
    SDL_Texture *blockTexture = loadTexture(renderer, "block.png");

    bool running = true;
    SDL_Event event;

    int playerX = 350, playerY = 500, playerSpeed = 5;
    SDL_FRect player = {(float) playerX, (float) playerY, 50, 50}; // Player size

    Uint32 startTime = SDL_GetTicks(); // Get start time

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        if (!running) { break; }

        move_player(playerSpeed, player);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Draw danger blocks
        if (rand() % 100 < 2) spawnBlock(); // 2% chance per frame to spawn a block
        updateBlocks();
        renderBlocks(renderer, blockTexture);

        // Draw player (green square)
        SDL_RenderTexture(renderer, playerTexture, nullptr, &player);

        SDL_RenderPresent(renderer);
        SDL_Delay(16); // ~60 FPS

        running = check_collisions(player);

        if (!running) {
            running = handle_restart(event, startTime);
            // Reset start time in a hacky fashion
            startTime = SDL_GetTicks();
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(playerTexture);
    SDL_DestroyTexture(blockTexture);
    SDL_Quit();
    return 0;
}
