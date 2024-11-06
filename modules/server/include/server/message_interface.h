#pragma once

#include "shared/message_types.h"

namespace server
{

    /**
     * @brief Interface for sending messages to the client
     *
     * This abstracts the network layer from the game logic, allowing the game logic to send messages to the client without knowing how the messages are sent.
     */
    class MessageInterface
    {
    public:
        void send_message(shared::ServerToClientMessage message);
    };
} // namespace server
