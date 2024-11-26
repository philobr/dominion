#include <server/network/message_interface.h>
#include <shared/utils/logger.h>

namespace server
{
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void ImplementedMessageInterface::sendMessage(std::unique_ptr<shared::ServerToClientMessage> message,
                                                   const shared::PlayerBase::id_t &player_id)
    {
        LOG(INFO) << "Message Interface called";
        std::string address = BasicNetwork::getInstance()->getAddress(player_id);
        std::string msg = message->toJson();
        LOG(INFO) << "Message Interface sending message: " << msg;

        BasicNetwork::getInstance()->sendMessage(msg, address);
        LOG(INFO) << "Message Interface done sending";
    }
} // namespace server