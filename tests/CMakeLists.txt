cmake_minimum_required(VERSION 3.16)

project(Tests)

# Download and include GoogleTest
include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE  # Fix for the warning
)

# Set up GoogleTest
FetchContent_MakeAvailable(googletest)

# Enable GoogleTest for tests
enable_testing()

# Include GoogleTest in the tests
add_subdirectory(client_tests)
add_subdirectory(server_tests)
add_subdirectory(shared_tests)
