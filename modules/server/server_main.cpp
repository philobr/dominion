
#include <server/args.h>
#include <server/network/server_network_manager.h>

#include <shared/utils/logger.h>

int main(int argc, char *argv[])
{
    server::ServerArgs args(argc, argv);

    shared::Logger::initialize();
    shared::Logger::setLevel(args.getLogLevel());
    shared::Logger::writeTo(args.getLogFile());

    while ( true ) {
        try {
            server::ServerNetworkManager server;
            server.run(args.getPort());
        } catch ( const std::exception &e ) {
            LOG(ERROR) << "Unhandled exception: " << e.what();
            LOG(DEBUG) << "Restarting server...";
        }
    }

    return 0;
}
