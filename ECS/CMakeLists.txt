cmake_minimum_required(VERSION 4.0)
project(ECS)

set(CMAKE_CXX_STANDARD 20)

# If SDL3 is not in a standard path
set(CMAKE_PREFIX_PATH "${PROJECT_SOURCE_DIR}/third_party/SDL3" ${CMAKE_PREFIX_PATH})
include_directories(${SDL3_INCLUDE_DIRS})
find_package(SDL3 CONFIG REQUIRED COMPONENTS SDL3-shared)

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

target_link_libraries(ECS PRIVATE SDL3::SDL3)

add_custom_command(TARGET ECS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${PROJECT_SOURCE_DIR}/third_party/SDL3/bin/SDL3.dll"
        $<TARGET_FILE_DIR:ECS>)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")