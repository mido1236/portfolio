add_executable(football_tests src/test_input_window.cpp src/test_disconnect.cpp
        src/test_input_integrity.cpp
        src/publisher.cpp
        include/publisher.h
        src/test_queue_limits.cpp
        src/test_process_limit.cpp)

# Discover tests for `ctest`
include(GoogleTest)
gtest_discover_tests(football_tests)
target_compile_definitions(football_tests PRIVATE FOOTBALL_TESTING)

target_link_libraries(football_tests PRIVATE football_core football_server_lib GTest::gtest GTest::gtest_main)
