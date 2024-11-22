add_executable(server_tests
    lobbies/lobby_lobbymanager.cpp
 
    game/cards/behaviour.cpp
    game/cards/card.cpp

    game/gamestate/server_player.cpp
    game/gamestate/server_board.cpp
    game/gamestate/server_gamestate.cpp
)

include_gtest(server_tests)
include_gmock(server_tests)
include_shared_lib(server_tests)
include_server_lib(server_tests)
include_rapidjson(server_tests)

add_test(NAME ServerTests COMMAND server_tests)
