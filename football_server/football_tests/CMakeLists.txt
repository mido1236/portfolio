add_executable(football_tests src/test_input_window.cpp)

target_link_libraries(football_tests PRIVATE football_core GTest::gtest GTest::gtest_main)

# Discover tests for `ctest`
include(GoogleTest)
gtest_discover_tests(football_tests)