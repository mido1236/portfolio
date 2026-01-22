cmake_minimum_required(VERSION 3.25)
project(football_server_lib)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(concurrentqueue REQUIRED)
find_package(unofficial-uwebsockets CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)

add_subdirectory(football_core)
add_subdirectory(football_server_lib)
add_subdirectory(football_server)
add_subdirectory(football_tests)
