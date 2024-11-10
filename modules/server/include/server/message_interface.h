#pragma once

#include "shared/message_types.h"

namespace server
{
    class MessageInterface
    {
    public:
        /**
         * @brief gets a response that has to be broadcast to all players except for the current player, passes it on to
         * the network manager as json
         */
        static void broadcast_message(std::unique_ptr<shared::ServerToClientMessage> message);
        /**
         * @brief gets a response that has to be sent to a client and passes it to the network manager as json string
         */
        // TODO: I think this also needs to take in a player ID
        static void send_message(std::unique_ptr<shared::ServerToClientMessage> message);
        /**
         * @brief Handles the request from the client and passes it on
         */
        static void handle_request(std::unique_ptr<shared::ClientToServerMessage> request);
    };
} // namespace server
