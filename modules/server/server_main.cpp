#include <string>

#include "server/args.h"
#include "server/network/server_network_manager.h"

int main(int argc, char* argv[])
{
    server::ServerArgs args(argc, argv);

    shared::Logger::initialize(args.getLogFile());

    // create a message interface, which creates a server network manager, which listens endlessly for connections
    server::ServerNetworkManager server;

    return 0;
}
