# modules/client/CMakeLists.txt

file(GLOB CLIENT_LIBRARY_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/panels/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/uiElements/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/windows/*.cpp"
)

# create lib
add_library(client_lib STATIC ${CLIENT_LIBRARY_SOURCES})

# add libs as needed for the lib
include_rapidjson(client_lib)  
include_wxwidgets(client_lib)  
include_sockpp(client_lib)
include_shared_lib(client_lib)

# expose headers
target_include_directories(client_lib PUBLIC
    ${CMAKE_SOURCE_DIR}/modules/client/include  
)

set(CLIENT_EXECUTABLE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_main.cpp  
    ${CMAKE_CURRENT_SOURCE_DIR}/src/client_network_manager.cpp
    PARENT_SCOPE
)
