add_executable(football_server)

target_sources(football_server PRIVATE src/main.cpp)

target_link_libraries(football_server PRIVATE football_server_lib)

add_custom_command(TARGET football_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_SOURCE_DIR}/certs/cacert.pem
        $<TARGET_FILE_DIR:football_server>/cacert.pem
)