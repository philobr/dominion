#include <string>

#include "server/network/server_network_manager.h"

#include <server/game/game_state/game_interface.h>
#include <shared/utils/test_helpers.h>
int main()
{
    // create a message interface, which creates a server network manager, which listens endlessly for connections

    auto behaviour_registry = server::BehaviourRegistry();

    //  server::ServerNetworkManager server;

    return 0;
}