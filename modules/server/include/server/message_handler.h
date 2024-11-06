
#include <server/lobbies.h>

#pragma once

namespace server
{

    /**
     * @brief Abstraction layer for handling messages
     *
     * This class is responsible for handling messages received over the network.
     * It will parse the message and call the appropriate functions in the LobbyManager.
     */
    class MessageHandler
    {
    public:
        MessageHandler();

        /**
         * @brief Handle a JSON message
         *
         * This function will parse the message and call the appropriate functions in the LobbyManager.
         */
        void HandleMessage(const std::string &message);

    private:
        LobbyManager lobby_manager;
    };
} // namespace server
