cmake_minimum_required(VERSION 3.10)
project(SDLStarter)

set(CMAKE_CXX_STANDARD 20)

# Set SDL3 directory
set(SDL3_DIR "C:/SDL3")

# Include SDL3 headers
include_directories(${SDL3_DIR}/include)

# Link to SDL3 library
link_directories(${SDL3_DIR}/lib)

# Define the executable
add_executable(SDLStarter main.cpp)

# Link the SDL3 library
target_link_libraries(SDLStarter SDL3 SDL3_image)

# Specify asset files to copy
set(DLLS SDL3.dll SDL3_image.dll)  # Add all required PNGs here

#set(SDL3_DLL_PATH "C:/SDL3/bin/SDL3.dll")  # Path to the DLL
set(SDL3_BINARY_DIR "C:/SDL3/bin")  # Ensures it copies to the build folder

# Copy assets to the output directory
foreach (dll ${DLLS})
    add_custom_command(
            TARGET SDLStarter POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${SDL3_BINARY_DIR}/${dll}
            ${CMAKE_BINARY_DIR}
    )
endforeach ()

# Specify asset files to copy
set(ASSETS player.png block.png)  # Add all required PNGs here

# Copy assets to the output directory
foreach (asset ${ASSETS})
    add_custom_command(
            TARGET SDLStarter POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_SOURCE_DIR}/${asset}
            ${CMAKE_BINARY_DIR}
    )
endforeach ()