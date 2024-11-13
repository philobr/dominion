#include <iostream>
#include <thread>

#include "client/client_listener.h"
#include "client/client_network_manager.h"


// wxIMPLEMENT_APP(client::Dominion);

// Temporary test function
int main()
{
    std::string input, host = "127.0.0.1";
    uint16_t port = 50505;
    std::cout << "127.0.0.1 50505" << std::endl;
    //std::cin >> host >> port;

    ClientNetworkManager network;
    network.init(host, port);

    input = ".";
    std::cout << "Initialized!" << std::endl;

    while ( input != "exit" ) {
        std::cin >> input;
        // network.sendRequest(input);
        std::cout << input;
    }

    return 0;
}