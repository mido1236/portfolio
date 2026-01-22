add_library(football_server_lib
        include/football_server/Messages.h)

target_sources(football_server_lib PRIVATE
        src/InBoundQueue.cpp
        src/GameServer.cpp
        src/UwsHub.cpp
        include/football_server/Topics.h
        include/football_server/Parsing.h
)

target_include_directories(football_server_lib PUBLIC include)

target_link_libraries(football_server_lib PUBLIC football_core unofficial::uwebsockets::uwebsockets concurrentqueue::concurrentqueue)

#get_target_property(UWS_INCLUDES unofficial::uwebsockets::uwebsockets INTERFACE_INCLUDE_DIRECTORIES)
#if (UWS_INCLUDES)
#    target_include_directories(football_server_lib SYSTEM PRIVATE ${UWS_INCLUDES})
#endif ()

if (MSVC)
    target_compile_options(football_server_lib PRIVATE /external:W0)
else ()
    target_compile_options(football_server_lib PRIVATE -Wall -Wextra -Wpedantic)
endif ()

#target_compile_definitions(football_server_lib
#        PRIVATE
#        $<$<BOOL:${CLION_IDE}>:FOOTBALL_TESTING>
#)