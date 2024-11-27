# modules/server/CMakeLists.txt

################################
# BUILD LIBRARY
################################

file(GLOB SERVER_LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/game/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/lobbies/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/network/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# create lib
add_library(server_lib ${SERVER_LIBRARY_SOURCES})

# expose headers
target_include_directories(server_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  
)

# add includes as needed for the lib
include_rapidjson(server_lib)   
include_sockpp(server_lib)
include_shared_lib(server_lib)
include_quick_arg_parser(server_lib)

set(SERVER_EXECUTABLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/server_main.cpp 
 
    PARENT_SCOPE
)
