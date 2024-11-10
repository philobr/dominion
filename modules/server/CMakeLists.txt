# modules/server/CMakeLists.txt

################################
# BUILD LIBRARY
################################

file(GLOB SERVER_LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# create lib
add_library(server_lib STATIC ${SERVER_LIBRARY_SOURCES})

# add includes as needed for the lib
include_rapidjson(server_lib)   
include_sockpp(server_lib)
include_shared_lib(server_lib)

# expose headers
target_include_directories(server_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/modules/server/include  
)

set(SERVER_EXECUTABLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server_main.cpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/server_network_manager.cpp
    PARENT_SCOPE
)
