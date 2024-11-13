//#include "server/message_interface.h"
#include <server/server_network_manager.h>

namespace server{
    ServerNetworkManager* MessageInterface::_network_manager = nullptr;
    void MessageInterface::init(){
        std::cout << "initializing server" << std::endl;
        ServerNetworkManager server;
        std::cout << "initialized server" << std::endl;
        MessageInterface::_network_manager = &server;
    }
    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id) {
        _network_manager->send_message(std::move(message), player_id);
    }

} // namespace server