#pragma once

#include <shared/message_types.h>

namespace server
{
    class MessageInterface
    {
    public:
        void send_message(shared::ServerToClientMessage *message, shared::PlayerBase::id_t player_id);
    };
} // namespace server
