
#include <memory>
#include <mutex>
#include <server/lobbies.h>
#include <shared/message_types.h>

#pragma once

namespace server
{

    /**
     * @brief Abstraction layer for handling messages
     *
     * This class is responsible for handling messages received over the network.
     * It will call the appropriate functions in the LobbyManager.
     */
    class MessageHandler
    {
    public:
        MessageHandler(std::shared_ptr<MessageInterface> message_interface) : lobby_manager_(message_interface) {}
        ~MessageHandler() {}

        /**
         * @brief Handle a JSON message
         *
         * This function will call the appropriate functions in the LobbyManager.
         */
        void HandleMessage(std::shared_ptr<MessageInterface> message_interface, std::unique_ptr<shared::ClientToServerMessage> message);

    private:
        LobbyManager lobby_manager_;
        std::mutex mutex_;
    };
} // namespace server
