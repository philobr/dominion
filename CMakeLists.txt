cmake_minimum_required(VERSION 3.10)
project(doMINION)

# C++ standard
set(CMAKE_CXX_STANDARD 17)

# Define the build types: Debug and Release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build: Debug or Release" FORCE)
endif ()

# Add common compiler options
add_compile_options(-Wall -Werror)

# Set build flags for Debug and Release modes
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Define options for building specific components
option(BUILD_CLIENT "Build the client executable" ON)
option(BUILD_SERVER "Build the server executable" ON)
option(BUILD_SHARED "Build the shared library" ON)

# Add subdirectories for client, server, shared, and tests
if (BUILD_CLIENT)
    add_subdirectory(src/client)
endif ()

if (BUILD_SERVER)
    add_subdirectory(src/server)
endif ()

if (BUILD_SHARED)
    add_subdirectory(src/shared)
endif ()

# Add the tests subdirectory, always build tests
add_subdirectory(tests)

# Meta-target to build everything at once
add_custom_target(build_all
        DEPENDS client server shared_library
)

# Enable testing
enable_testing()
