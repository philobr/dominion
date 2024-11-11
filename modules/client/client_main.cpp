#include <wx/wx.h>
#include "src/dominion.h"

#include <iostream>
#include <thread>

#include "client/client_listener.h"
#include "client/client_network_manager.h"


//wxIMPLEMENT_APP(client::Dominion);

//Temporary test function
int main()
{
    std::string input, host;
    uint16_t port;
    std::cin >> host >> port;

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
