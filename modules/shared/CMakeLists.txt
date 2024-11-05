# modules/shared/CMakeLists.txt

add_library(shared_lib STATIC
    src/example.cpp
)

# Add root/modules/shared/include as the public include directory
target_include_directories(shared_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/modules/shared/include
)
