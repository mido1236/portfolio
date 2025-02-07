# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project Name
project(ThreadPoolProject LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include header files
include_directories(include)

# Source files
set(SOURCES src/main.cpp)

# Create the executable
add_executable(ThreadPool ${SOURCES})

# Enable threading support
find_package(Threads REQUIRED)
target_link_libraries(ThreadPool PRIVATE Threads::Threads)

