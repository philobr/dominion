#include <string>

#include "server/network/server_network_manager.h"

#include <server/game/game_state/game_interface.h>
#include <shared/utils/test_helpers.h>

int main()
{
    // initialise the logger to log to build/logs/
    shared::Logger::initialize("server.log");

    std::vector<std::string> player_ids = {"player1", "player2", "player3"};
    auto valid_cards = get_valid_kingdom_cards();
    auto interface = server::GameInterface::make("game_id", valid_cards, player_ids);

    // create a message interface, which creates a server network manager, which listens endlessly for connections
    // server::ServerNetworkManager server;

    return 0;
}