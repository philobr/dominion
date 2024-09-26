# from gpt:)

add_executable(client
        main_client.cpp
)

target_link_libraries(client PUBLIC shared_library) # Link the shared library
