# modules/shared/CMakeLists.txt

add_library(shared_lib STATIC
    src/message_types/from_json.cpp
    src/message_types/to_json.cpp
)

include_rapidjson(shared_lib)

# Add root/modules/shared/include as the public include directory
target_include_directories(shared_lib
    PUBLIC ${CMAKE_SOURCE_DIR}/modules/shared/include
)
