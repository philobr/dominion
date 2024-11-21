#include <server/network/message_interface.h>

namespace server
{
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void ImplementedMessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                                                   const shared::PlayerBase::id_t &player_id)
    {
        std::string address = BasicNetwork::getInstance()->get_address(player_id);
        std::string msg = message->to_json();

        BasicNetwork::getInstance()->send_message(msg, address);
    }
} // namespace server