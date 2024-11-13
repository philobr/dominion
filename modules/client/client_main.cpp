#include <iostream>
#include <thread>
#include <wx/wx.h>

#include "client/client_listener.h"
#include "client/client_network_manager.h"
#include "shared/message_types.h"


// wxIMPLEMENT_APP(client::Dominion);

// Temporary test function
int main()
{
    //Initialize wxWidgets
    wxInitializer initializer;
    if(!initializer){
        std::cerr << "Failed to initialize wxWidgets, aborting." << std::endl;
        return -1;
    }

    std::string input, host = "127.0.0.1";
    uint16_t port = 50505;
    std::cout << "127.0.0.1 50505" << std::endl;
    //std::cin >> host >> port;

    ClientNetworkManager network;
    network.init(host, port);

    input = ".";
    std::cout << "Initialized!" << std::endl;

   shared::CreateLobbyRequestMessage req("Bob", "Stuart", "Kevin");
   std::cout << "Created CreateLobbyRequestMessage" << std::endl;

   network.sendRequest(std::make_unique<shared::CreateLobbyRequestMessage>(req));
   std::cout << "Sent Request" << std::endl;

   std::cout << "Waiting for exit signal(type exit):" << std::endl;
    while ( input != "exit" ) {
        std::cin >> input;
    }

    network.shutdown();
    std::cout << "Shutdown ClientNetworkManager" << std::endl;

    return 0;
}