add_executable(server_tests
    test.cpp
    lobby_lobbymanager.cpp
    # add testfiles here
)

include_gtest(server_tests)
include_gmock(server_tests)
include_shared_lib(server_tests)
include_server_lib(server_tests)

add_test(NAME ServerTests COMMAND server_tests)
