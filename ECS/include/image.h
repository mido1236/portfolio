//
// Created by ahmed on 2025-10-01.
//

#ifndef ECS_IMAGE_H
#define ECS_IMAGE_H
#include <string>

#include "SDL3/SDL_render.h"
#include "SDL3_image/SDL_image.h"

inline SDL_Texture *loadTexture(SDL_Renderer *renderer, const std::string &filePath) {
    SDL_Texture *tex = IMG_LoadTexture(renderer, filePath.c_str());
    if (!tex) {
        SDL_Log("Failed to load texture %s", filePath.c_str());
        return nullptr;
    }
    return tex;
}
#endif //ECS_IMAGE_H
