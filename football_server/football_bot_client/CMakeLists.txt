add_executable(football_bot_client
        include/client)

target_sources(football_bot_client PRIVATE src/main.cpp
        src/SimClient.cpp)

target_link_libraries(football_bot_client PUBLIC Bcrypt ixwebsocket::ixwebsocket football_core)

target_include_directories(football_bot_client PUBLIC include)
