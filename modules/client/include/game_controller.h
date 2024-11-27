#pragma once

#include "client_network_manager.h"
#include "panels/connection_panel.h"
#include "panels/lobby_panel.h"
#include "panels/main_game_panel.h"
#include "shared/game/game_state/player_base.h"
#include "windows/game_window.h"

#include <shared/message_types.h>

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
         * @brief Get all the necessary information to create a request message
         */
        static bool validInput(const wxString &inputServerAddress, const wxString &inputServerPort,
                               const wxString &inputPlayerName, const wxString &inputGameName);

        /**
         * @brief Show the connection panel and send create lobby request
         */
        static void createLobby();

        /**
         * @brief Join a Lobby
         */
        static void joinLobby();

        /**
         * @brief Show the main game panel
         *
         */
        static void startGame();

        static void buyCard(const std::string &card_id);
        static void playCard(unsigned int cardIndex);
        static void endTurn();
        static void sendRequest(const std::string &req);

        /**
         * @brief Receive a message from the server
         * @param msg The message received
         */
        static void receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg);

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

        static void refreshPlayers(shared::JoinLobbyBroadcastMessage &msg);

    private:
        static void receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> msg);
        static void receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg);
        static void receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg);
        static void receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg);

        static shared::PlayerBase::id_t getPlayerName();

        static GameWindow *_gameWindow;
        static ConnectionPanel *_connectionPanel;
        static MainGamePanel *_mainGamePanel;
        static LobbyPanel *_lobbyPanel;
        static ClientNetworkManager *_clientNetworkManager;

        static std::unique_ptr<reduced::GameState> _gameState;
        static shared::PlayerBase::id_t _playerName;
        static std::string _gameName;
    };

} // namespace client
