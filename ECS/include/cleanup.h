//
// Created by ahmed on 2025-09-10.
//

#ifndef ECS_CLEANUP_H
#define ECS_CLEANUP_H

#include <csignal>
#include <iostream>
#include <conio.h>

namespace Game {
    inline void cleanupConsole() {
        // Clear + reset cursor
        // std::cout << "\033[2J\033[H";
        // std::cout << "\033[?25h"; // make sure cursor visible again
        std::cout.flush();

        // Flush stray keystrokes
        // while (_kbhit()) _getch();
    }

    inline void shutdown() {
        cleanupConsole();
        std::cout << "Game Over. Thanks for playing!\n";
    }

    inline void signalHandler(int signal) {
        shutdown();
        std::_Exit(signal); // immediate exit, bypass destructors
    }

    inline void setupSignalHandlers() {
        std::signal(SIGINT, signalHandler);  // CTRL+C
        std::signal(SIGTERM, signalHandler); // generic termination
    }

} // namespace Game

#endif //ECS_CLEANUP_H