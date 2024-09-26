# from gpt:)

add_executable(server
        main_server.cpp
)

target_link_libraries(server PUBLIC shared_library) # Link the shared library
