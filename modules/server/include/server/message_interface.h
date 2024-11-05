#pragma once

#include "shared/message_types.h"

namespace server
{
    class MessageInterface
    {
    public:
        void send_message(shared::ServerToClientMessage message);
    };
} // namespace server
