#pragma once

#include <shared/message_types.h>
#include "gmock/gmock.h"

namespace server
{

    /**
     * @brief Interface for sending messages to the client
     *
     * This abstracts the network layer from the game logic, allowing the game logic to send messages to the client
     * without knowing how the messages are sent.
     */
    class MessageInterface
    {
    public:
        virtual ~MessageInterface() = default;
        virtual void send_message(shared::ServerToClientMessage *message, shared::PlayerBase::id_t player_id) = 0;
    };

    class ImplementedMessageInterface : public MessageInterface
    {
    public:
        void send_message(shared::ServerToClientMessage *message, shared::PlayerBase::id_t player_id) override{
            return; // TODO: Implement
        };
    };

    class MockMessageInterface : public MessageInterface {
        public:
        // Mock the send_message method, assuming it takes these parameters
                MOCK_METHOD(void, send_message, (shared::ServerToClientMessage* message, shared::PlayerBase::id_t player_id), (override));
    };

} // namespace server
