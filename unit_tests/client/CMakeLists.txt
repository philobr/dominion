# unit_tests/client_tests/CMakeLists.txt

# Collect all test source files
file(GLOB_RECURSE TEST_SOURCES *.cpp)

# Create the test executable
add_executable(client_tests ${TEST_SOURCES})

# Use the macros to include the necessary libraries and headers
include_client_lib(client_tests)  # Link against client_lib and include its headers
include_shared_lib(client_tests)   # Link against shared_lib and include its headers
include_gtest(client_tests)        # Link against Google Test

# Register the test with CTest
add_test(NAME ClientTests COMMAND client_tests)

target_include_directories(client_tests PRIVATE ${CMAKE_SOURCE_DIR}/modules/client/include)
