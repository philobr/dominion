#pragma once

#include <../../../../external/googletest/lib/googlemock/include/gmock/gmock.h>
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
        virtual void send_message(shared::ServerToClientMessage *message, shared::PlayerBase::id_t player_id) = 0;
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
        void send_message(shared::ServerToClientMessage *message, shared::PlayerBase::id_t player_id) override
        {
            return; // TODO: Implement
        };
    };

    /**
     * @brief Mock implementation of the MessageInterface
     *
     * @details Used for testing the game logic without actually sending messages to the client
     */
    class MockMessageInterface : public MessageInterface
    {
    public:
        // Mock the send_message method, assuming it takes these parameters
        MOCK_METHOD(void, send_message, (shared::ServerToClientMessage * message, shared::PlayerBase::id_t player_id),
                    (override));
    };

} // namespace server
