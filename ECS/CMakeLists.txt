cmake_minimum_required(VERSION 4.0)
project(ECS)

set(CMAKE_CXX_STANDARD 20)

include(cmake/CPM.cmake)

set(ARCH_SUBDIR "x86_64-w64-mingw32")
if (MSVC)
    set(ARCH_SUBDIR "")
endif ()

function(fetch_sdl3 pkg version)
    CPMAddPackage(
            NAME SDL3${pkg}
            URL "https://github.com/libsdl-org/SDL${pkg}/releases/download/release-${version}/SDL3${pkg}-devel-${version}-mingw.zip"
            DOWNLOAD_ONLY YES
    )
    set(SDL3${pkg}_DIR ${SDL3${pkg}_SOURCE_DIR}/${ARCH_SUBDIR} PARENT_SCOPE)
endfunction()

fetch_sdl3("" 3.2.24)
fetch_sdl3(_ttf 3.2.2)
fetch_sdl3(_image 3.2.4)

add_executable(ECS src/main.cpp
        include/components.h
        include/ecs.h
        include/component_array.h
        include/entity.h
        src/render_system.cpp
        src/player.cpp
        include/movement.h
        include/collision.h
        include/attack_system.h
        include/Input.h
        include/player_movement.h
        include/projectile_system.h
        include/spawner_system.h
        include/cleanup.h
        include/custom_random.h
        include/image.h
        include/destruction_system.h
        include/event_system.h
)

list(PREPEND CMAKE_PREFIX_PATH "${SDL3_DIR}" "${SDL3_image_DIR}" "${SDL3_ttf_DIR}")

add_custom_command(TARGET ECS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_DIR}/bin/SDL3.dll"
        $<TARGET_FILE_DIR:ECS>)

add_custom_command(TARGET ECS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_image_DIR}/bin/SDL3_image.dll"
        $<TARGET_FILE_DIR:ECS>)

add_custom_command(TARGET ECS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL3_ttf_DIR}/bin/SDL3_ttf.dll"
        $<TARGET_FILE_DIR:ECS>)

add_custom_command(TARGET ECS POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        $<TARGET_FILE_DIR:ECS>/assets)

find_package(SDL3 CONFIG REQUIRED COMPONENTS SDL3-shared)
find_package(SDL3_image CONFIG REQUIRED COMPONENTS SDL3_image-shared)
find_package(SDL3_ttf CONFIG REQUIRED COMPONENTS SDL3_ttf-shared)

target_link_libraries(ECS PRIVATE SDL3::SDL3 SDL3_image::SDL3_image SDL3_ttf::SDL3_ttf)

set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")