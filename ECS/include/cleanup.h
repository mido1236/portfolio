//
// Created by ahmed on 2025-09-10.
//

#ifndef ECS_CLEANUP_H
#define ECS_CLEANUP_H

#include <csignal>
#include <iostream>
#include <conio.h>

namespace Game {
    inline void shutdown() {
        std::cout << "Game Over. Thanks for playing!\n";
    }

    inline void signalHandler(const int signal) {
        shutdown();
        std::_Exit(signal); // immediate exit, bypass destructors
    }

    inline void setupSignalHandlers() {
        std::signal(SIGINT, signalHandler); // CTRL+C
        std::signal(SIGTERM, signalHandler); // generic termination
    }
} // namespace Game

#endif //ECS_CLEANUP_H