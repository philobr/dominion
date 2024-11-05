#pragma once

#include <string>

#include "shared/message_types.h"

#include "lobbies.h"

namespace server
{
    class ServerNetworkManager
    {
    public:
        void handle_message(std::string message);
        void send_message(shared::ServerToClientMessage message);
        MessageInterface get_message_interface();

    private:
        LobbyManager lobby_manager;
    };
} // namespace server
