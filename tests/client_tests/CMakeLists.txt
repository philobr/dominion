# Add client test executable
add_executable(client_tests client_tests.cpp)

# Link the test executable to client_lib and GoogleTest
target_link_libraries(client_tests client_lib gtest gtest_main)

# Add GoogleTest integration
include(GoogleTest)
gtest_discover_tests(client_tests)

