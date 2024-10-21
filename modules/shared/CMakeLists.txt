project(${CMAKE_PROJECT_NAME}/Shared)

# Declare shared as a header-only library
add_library(shared INTERFACE)

# Add the include directories to the INTERFACE target
target_include_directories(shared INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)