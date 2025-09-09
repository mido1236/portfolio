#include <conio.h>
#include "Input.h"

Input inputSystem() {
    Input input;

    if (!_kbhit()) return input;

    switch (_getch()) {
        case 'w': input.up = true;
            break;
        case 's': input.down = true;
            break;
        case 'a': input.left = true;
            break;
        case 'd': input.right = true;
            break;
        case ' ': input.attack = true;
        default: ;
    }

    return input;
}
