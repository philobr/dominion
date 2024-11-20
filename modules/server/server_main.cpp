#include <string>

#include "server/network/server_network_manager.h"

#include <shared/utils/test.h>
int main()
{
    // create a message interface, which creates a server network manager, which listens endlessly for
    // connections server::ServerNetworkManager server;
    shared::Logger::initialize("test.log");
    LOG(INFO) << "info?";
    LOG(WARN) << "test?";
    LOG(ERROR) << "error?";
    LOG(DEBUG) << "debug?";

    test("is it working if only defined in main, but called in another file?");

    return 0;
}