cmake_minimum_required(VERSION 4.0)
project(ECS)

set(CMAKE_CXX_STANDARD 20)

add_executable(ECS main.cpp
        components.h
        ecs.h
        component_array.h
        entity.h
        render_system.cpp
        player.cpp
        movement.h
        collision.h
        attack_system.h
        Input.h
        player_movement.h
        projectile_system.h
        spawner_system.h
        cleanup.h
        custom_random.h)
