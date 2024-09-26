# from gpt:)

# Download and configure GoogleTest
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

# Add an executable for tests
add_executable(runTests
        client_tests.cpp
        server_tests.cpp
        shared_tests.cpp
)

# Link GoogleTest and the shared code library
target_link_libraries(runTests
        GTest::gtest_main
        shared_library  # Link the shared code between client and server
)

include(GoogleTest)
gtest_discover_tests(runTests)
