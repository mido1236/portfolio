#include <conio.h>
#include <windows.h>

#include "Input.h"

bool isKeyDown(const int vk) {
    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

Input inputSystem() {
    Input input;

    input.up = (GetAsyncKeyState('W') & 0x8000) != 0;
    input.down = (GetAsyncKeyState('S') & 0x8000) != 0;
    input.left = (GetAsyncKeyState('A') & 0x8000) != 0;
    input.right = (GetAsyncKeyState('D') & 0x8000) != 0;
    input.attack = (GetAsyncKeyState(' ') & 0x8000) != 0;

    return input;
}
