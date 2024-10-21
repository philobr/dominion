# Add server test executable
add_executable(shared_tests shared_tests.cpp)

# Link the test executable to server_lib and GoogleTest
target_link_libraries(shared_tests shared gtest_main)

# Add GoogleTest integration
include(GoogleTest)
gtest_discover_tests(shared_tests)