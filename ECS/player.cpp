#include <SDL3/SDL.h>
#include <conio.h>
#include <windows.h>

#include "Input.h"

bool isKeyDown(const int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

Input inputSystem() {
    Input input;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) input.quit = true;
        if (event.type == SDL_EVENT_KEY_DOWN) {
            if (event.key.key == SDLK_ESCAPE) input.quit = true;
        }
    }

    const bool *state = SDL_GetKeyboardState(nullptr);
    // WASD movement
    if (state[SDL_SCANCODE_W]) input.up = true;
    if (state[SDL_SCANCODE_S]) input.down = true;
    if (state[SDL_SCANCODE_A]) input.left = true;
    if (state[SDL_SCANCODE_D]) input.right = true;
    if (state[SDL_SCANCODE_SPACE]) input.attack = true;

    return input;
}
