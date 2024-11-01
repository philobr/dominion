# modules/client/CMakeLists.txt

set(CLIENT_LIBRARY_SOURCES
    src/example.cpp  
    # add non main files here
)

# create lib
add_library(client_lib ${CLIENT_LIBRARY_SOURCES})

# expose headers
target_include_directories(client_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include  
)

# add libs as needed for the lib
include_rapidjson(client_lib)  
include_wxwidgets(client_lib)  
include_sockpp(client_lib)
include_shared_lib(client_lib)

set(CLIENT_EXECUTABLE_SOURCES
    src/client_main.cpp  
)

add_executable(client_exe ${CLIENT_EXECUTABLE_SOURCES})
target_link_libraries(client_exe PRIVATE client_lib)

# add libs as needed for the exe
include_shared_lib(client_exe)
