#include <iostream>
#include <thread>

#include "client/client_listener.h"
#include "client/client_network_manager.h"

void set_up_network(std::string host, uint16_t port)
{
    ClientNetworkManager network;
    network.init(host, port);
}

int main()
{
    std::string input, host;
    uint16_t port;
    std::cin >> host >> port;

    std::thread thread(set_up_network, host, port);
    thread.detach();

    input = ".";
    std::cout << "Initialized!" << std::endl;

    while ( input != "exit" ) {
        std::cin >> input;
        // network.sendRequest(input);
        std::cout << input;
    }
    return 0;
}
