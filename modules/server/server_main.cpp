#include <string>

#include "server/network/server_network_manager.h"

#include <server/game/game_state/game_interface.h>
#include <shared/utils/test_helpers.h>
int main()
{
    // create a message interface, which creates a server network manager, which listens endlessly for connections

    const std::vector<std::string> player_ids = {"one", "two"};
    const auto kcards = get_valid_kingdom_cards();
    auto interface = server::GameInterface::make("test", kcards, player_ids);

    //  server::ServerNetworkManager server;

    return 0;
}