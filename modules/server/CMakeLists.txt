# modules/server/CMakeLists.txt

set(SERVER_LIBRARY_SOURCES
    src/example.cpp  
    # add non main files here
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

set(SERVER_EXECUTABLE_SOURCES
    src/server_main.cpp  
)

add_executable(server_exe ${SERVER_EXECUTABLE_SOURCES})

# include libs
include_server_lib(server_exe)
include_sockpp(server_exe)