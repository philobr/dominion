#include "server/message_interface.h"

namespace server{
    MessageInterface::MessageInterface() = default;
    MessageInterface::~MessageInterface() = default;

    void MessageInterface::send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id) {
        //TODO Mathieu
    }

    void MessageInterface::handle_request(std::unique_ptr<shared::ClientToServerMessage> request){
        //TODO Nicholas
    }

} // namespace server