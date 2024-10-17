cmake_minimum_required(VERSION 3.16)
project(dominion)

set(CMAKE_CXX_STANDARD 20) 
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable testing
enable_testing()

# Add subdirectories for client, server, and shared
add_subdirectory(modules/client)
add_subdirectory(modules/server)
add_subdirectory(modules/shared)
add_subdirectory(tests)  # Test directory
