add_executable(football_server)

target_sources(football_server PRIVATE src/main.cpp)

target_link_libraries(football_server PRIVATE football_server_lib)