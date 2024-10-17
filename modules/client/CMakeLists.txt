project(${CMAKE_PROJECT_NAME}/Client)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add external libraries if any
# add_subdirectory(external_libraries)

# Add a library for the client code
add_library(client_lib STATIC src/client_core.cpp)  # src/client_code.cpp contains the core logic

# Add the executable for the client
add_executable(client src/main.cpp)

# Link the client executable to the shared client_lib
target_link_libraries(client client_lib shared)
