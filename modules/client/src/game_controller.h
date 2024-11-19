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
        /**
         * @brief Initialize the game controller
         *
         * @param gameWindow The main window of the game
         */
        static void init(GameWindow *gameWindow);

        /**
         * @brief Show the connection panel and send create lobby request
         * @TODO: Implement this function where only the create/join lobby
         * request is sent and no lobby panel is shown yet
         */
        static void connectToServer();

        /**
         * @brief Show the main game panel
         *
         */
        static void startGame();
        static void send_request(const std::string &req);
        static void receive_message(std::unique_ptr<shared::ServerToClientMessage> msg);

        /**
         * @brief display an error message
         *
         */
        static void showError(const std::string &title, const std::string &message);

        /**
         * @brief display a status message in the status bar
         *
         */
        static void showStatus(const std::string &message);

    private:
        static GameWindow *_gameWindow;
        static ConnectionPanel *_connectionPanel;
        static MainGamePanel *_mainGamePanel;
        static LobbyPanel *_lobbyPanel;
        static ClientNetworkManager *_clientNetworkManager;
    };

} // namespace client
