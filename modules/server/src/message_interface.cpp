#include <server/network/message_interface.h>

namespace server{
    std::shared_ptr<BasicNetwork> MessageInterface::basic_network = nullptr;
    MessageInterface::MessageInterface(std::shared_ptr<BasicNetwork> _basic_network)  {
        if(basic_network == nullptr){
            basic_network = _basic_network;
        }
    }
    MessageInterface::~MessageInterface() = default;
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id) {
        std::string address = basic_network.get()->get_address(player_id);
        std::string msg = message->to_json();

        basic_network.get()->send_message(msg, address);
    }

} // namespace server