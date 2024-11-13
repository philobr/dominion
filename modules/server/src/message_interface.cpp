//#include "server/message_interface.h"
#include <server/server_network_manager.h>

namespace server{
    std::unique_ptr<ServerNetworkManager> MessageInterface::_network_manager = nullptr;
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void MessageInterface::init(){
        ServerNetworkManager server;
        MessageInterface::_network_manager = std::make_unique<ServerNetworkManager>(server);
    }
    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id) {
        _network_manager->send_message(std::move(message), player_id);
    }

} // namespace server