project(${CMAKE_PROJECT_NAME}/Server)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add external libraries if any
# add_subdirectory(external_libraries)

# Add a library for the server code
add_library(server_lib STATIC src/server_core.cpp)  # src/server_code.cpp contains the core logic

# Add the executable for the server
add_executable(server src/main.cpp)

# Link the server executable to the shared server_lib
target_link_libraries(server server_lib shared)
