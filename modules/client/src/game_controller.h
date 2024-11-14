#pragma once

#include "client/client_network_manager.h"
#include "panels/connection_panel.h"
#include "panels/lobby_panel.h"
#include "panels/main_game_panel.h"
#include "windows/game_window.h"

namespace client
{

    class GameController
    {

    public:
        static void init(GameWindow *gameWindow);

        static void connectToServer();
        static void startGame();
        static void send_request(std::string req);
        static void receive_message(std::unique_ptr<shared::ServerToClientMessage> msg);

        static void showError(const std::string &title, const std::string &message);
        static void showStatus(const std::string &message);

    private:
        static GameWindow *_gameWindow;
        static ConnectionPanel *_connectionPanel;
        static MainGamePanel *_mainGamePanel;
        static LobbyPanel *_lobbyPanel;
        static ClientNetworkManager *_clientNetworkManager;
    };

} // namespace client
