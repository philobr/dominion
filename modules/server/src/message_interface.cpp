#include <server/message_interface.h>
#include <server/lobby_manager.h>

namespace server{
    MessageInterface::MessageInterface(){
        if(MessageInterface::_message_interface == nullptr){
            MessageInterface::_message_interface = std::make_shared<MessageInterface>(this);
        }
    }
    std::unique_ptr<ServerNetworkManager> MessageInterface::_network_manager = nullptr;
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void MessageInterface::init(){
        LobbyManager lobby_manager(MessageInterface::_message_interface);
        ServerNetworkManager server(lobby_manager);
        MessageInterface::_network_manager = std::make_unique<ServerNetworkManager>(server);
    }
    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id) {
        _network_manager->send_message(std::move(message), player_id);
    }

} // namespace server