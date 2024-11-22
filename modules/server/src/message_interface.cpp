#include <server/network/message_interface.h>
#include <shared/utils/logger.h>

namespace server
{
    MessageInterface::MessageInterface() = default;
    MessageInterface::~MessageInterface() = default;
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                                        const shared::PlayerBase::id_t &player_id)
    {
        LOG(INFO) << "Message Interface called";
        std::string address = BasicNetwork::getInstance()->get_address(player_id);
        std::string msg = message->to_json();
        LOG(INFO) << "Message Interface sending message: " << msg;

        BasicNetwork::getInstance()->send_message(msg, address);
        LOG(INFO) << "Message Interface done sending";
    }

} // namespace server