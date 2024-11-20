#include <string>

#include "server/network/server_network_manager.h"
int main()
{
    // create a message interface, which creates a server network manager, which listens endlessly for connections
    server::ServerNetworkManager server;

    return 0;
}