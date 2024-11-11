add_executable(server_tests
    lobby_lobbymanager.cpp
)

include_gmock(server_tests)
include_gtest(server_tests)
include_rapidjson(server_tests)
include_shared_lib(server_tests)
include_server_lib(server_tests)

add_test(NAME ServerTests COMMAND server_tests)
