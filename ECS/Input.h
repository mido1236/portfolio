//
// Created by ahmed on 2025-09-02.
//

#ifndef ECS_INPUT_H
#define ECS_INPUT_H

struct Input {
    bool up = false, down = false, left = false, right = false;
    bool attack = false;
    bool quit = false;
};

#endif //ECS_INPUT_H
