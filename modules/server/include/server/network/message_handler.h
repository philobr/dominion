#pragma once

#include <memory>
#include <mutex>

#include <server/lobbies/lobby_manager.h>
#include <shared/message_types.h>

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
        MessageHandler(LobbyManager lobby_manager) : lobby_manager_(lobby_manager) {};
        MessageHandler(const MessageHandler& msg_handler) : lobby_manager_(msg_handler.lobby_manager_) {};
        ~MessageHandler() = default;

        /**
         * @brief Handle a JSON message
         *
         * This function will call the appropriate functions in the LobbyManager.
         */
        void HandleMessage(std::unique_ptr<shared::ClientToServerMessage> message);

    private:
        LobbyManager lobby_manager_;
        std::mutex mutex_;
    };
} // namespace server
