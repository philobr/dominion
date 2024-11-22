#include <string>

#include <server/network/server_network_manager.h>
#include<shared/utils/logger.h>
int main()
{
    // create a message interface, which creates a server network manager, which listens endlessly for connections
    shared::Logger::initialize("server_log.log");
    server::ServerNetworkManager server;

    return 0;
}