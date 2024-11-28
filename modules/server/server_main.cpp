
#include <server/args.h>
#include <server/network/server_network_manager.h>

#include <shared/utils/logger.h>

int main(int argc, char *argv[])
{
    server::ServerArgs args(argc, argv);

    shared::Logger::initialize();
    shared::Logger::setLevel(args.getLogLevel());
    shared::Logger::writeTo(args.getLogFile());

    // In case the server crashes, we simply restart it
    // The server is completely reset, so all clients will be disconnected
    // This is not a problem, since the server is not supposed to crash in the first place
    while ( true ) {
        try {
            server::ServerNetworkManager server;
            server.run(server::DEFAULT_SERVER_HOST, args.getPort());
        } catch ( const std::exception &e ) {
            LOG(ERROR) << "Unhandled exception: " << e.what();
            LOG(DEBUG) << "Restarting server...";
        }
    }

    return 0;
}
