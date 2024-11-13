#pragma once

#include <shared/message_types.h>

namespace server
{

    /**
     * @brief Abstract base class for sending messages to the client
     *
     * @details Used for enabling Mocking of the send_message method
     * for writing tests.
     */
    class MessageInterface
    {
    public:
        virtual ~MessageInterface() = default;
        
        /**
         * @brief gets a response that has to be broadcast to all players except for the current player, passes it on to
         * the network manager as json
         */
        static void broadcast_message(std::unique_ptr<shared::ServerToClientMessage> message);
        
        /**
         * @brief gets a response that has to be sent to a client and passes it to the network manager as json string
         */
        // TODO: I think this also needs to take in a player ID
        virtual void send_message(std::unique_ptr<shared::ServerToClientMessage> message, const shared::PlayerBase::id_t& player_id);
        
        /**
         * @brief Handles the request from the client and passes it on
         */
        static void handle_request(std::unique_ptr<shared::ClientToServerMessage> request);
    };

    /**
     * @brief Interface for sending messages to the client
     *
     * This abstracts the network layer from the game logic, allowing the game logic to send messages to the client
     * without knowing how the messages are sent.
     */
    class ImplementedMessageInterface : public MessageInterface
    {
    public:
        void send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                          const shared::PlayerBase::id_t &player_id) override
        {
            return; // TODO: Implement
        };
    };

} // namespace server
