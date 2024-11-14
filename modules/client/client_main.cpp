#include <iostream>
#include <thread>
#include <wx/wx.h>

#include <client/client_listener.h>
#include <client/client_network_manager.h>
#include <shared/message_types.h>
#include <client/dominion.h>


wxIMPLEMENT_APP(client::Dominion);
/*
// Temporary test function
int main()
{
    // Initialize wxWidgets
    wxInitializer initializer;
    if ( !initializer ) {
        std::cerr << "Failed to initialize wxWidgets, aborting." << std::endl;
        return -1;
    }

    std::string input, host = "127.0.0.1";
    uint16_t port = 50505;
    std::cout << "Host: 127.0.0.1, Port: 50505" << std::endl;
    // std::cin >> host >> port;

    ClientNetworkManager network;
    network.init(host, port);

    //for testing purposes
    input = "";
    std::cout << "Initialized!" << std::endl;

   shared::CreateLobbyRequestMessage req("Bob", "Stuart", "Kevin");
   std::cout << "Created CreateLobbyRequestMessage" << std::endl;

   network.sendRequest(req.to_json());
   std::cout << "Sent Request" << std::endl;

   std::cout << "Waiting for exit signal(type exit):" << std::endl;
    while ( input != "exit" ) {
        std::cin >> input;
        if(input == "another"){
            std::cout << "sending another one" << std::endl;
            network.sendRequest(req.to_json());
            input = "";
        }
        std::cout << input << std::endl;
    }

    network.shutdown();
    std::cout << "Shutdown ClientNetworkManager" << std::endl;

    return 0;
}
*/