add_library(football_server)

target_sources(football_server PRIVATE
        src/main.cpp
        src/GameServer.cpp
        src/UwsHub.cpp
        include/Topics.h
        include/Parsing.h
)

target_include_directories(football_server PUBLIC
        include
)

target_link_libraries(football_server PRIVATE football_core unofficial::uwebsockets::uwebsockets concurrentqueue::concurrentqueue)

#get_target_property(UWS_INCLUDES unofficial::uwebsockets::uwebsockets INTERFACE_INCLUDE_DIRECTORIES)
#if (UWS_INCLUDES)
#    target_include_directories(football_server SYSTEM PRIVATE ${UWS_INCLUDES})
#endif ()

if (MSVC)
    target_compile_options(football_server PRIVATE /external:W0)
else ()
    target_compile_options(football_server PRIVATE -Wall -Wextra -Wpedantic)
endif ()