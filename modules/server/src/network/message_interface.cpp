#include <server/network/message_interface.h>
#include <shared/utils/logger.h>

namespace server
{
    /**
     * @brief Initializes the Message interface and thereby starts the server listener loop
     */
    void ImplementedMessageInterface::sendMessage(const shared::ServerToClientMessage &message,
                                                  const shared::PlayerBase::id_t &player_id)
    {
        std::string msg = message.toJson();

        LOG(INFO) << "Message Interface sending: " << msg;
        BasicNetwork::sendToPlayer(msg, player_id);
    }

} // namespace server
