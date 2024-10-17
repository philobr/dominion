# Add server test executable
add_executable(server_tests server_tests.cpp)

# Link the test executable to server_lib and GoogleTest
target_link_libraries(server_tests server_lib gtest gtest_main)

# Add GoogleTest integration
include(GoogleTest)
gtest_discover_tests(server_tests)
