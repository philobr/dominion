#pragma once

#include <server/server_network_manager.h>
#include <shared/message_types.h>

namespace server{
    /**
     * @brief Abstract base class for sending messages to the client
     *
     * @details Used for enabling Mocking of the send_message method
     * for writing tests.
     */
    class MessageInterface
    {
    public:
        MessageInterface();
        ~MessageInterface();

        static void init();
        /**
         * @brief gets a response that has to be sent to a client and passes it to the network manager as json string
         */
        // TODO: I think this also needs to take in a player ID
        static void send_message(std::unique_ptr<shared::ServerToClientMessage> message,
                                 const shared::PlayerBase::id_t &player_id);

    private:
        static std::unique_ptr<ServerNetworkManager> _network_manager;
        static std::shared_ptr<MessageInterface> _message_interface;
    };
} // namespace server