#pragma once

#include "client_network_manager.h"
#include "panels/connection_panel.h"
#include "panels/lobby_panel.h"
#include "panels/main_game_panel.h"
#include "shared/game/game_state/player_base.h"
#include "windows/game_window.h"

#include <panels/victory_screen_panel.h>

#include <shared/message_types.h>

namespace client
{

    enum ClientState
    {
        LOGIN_SCREEN,
        CREATING_LOBBY,
        JOINING_LOBBY,
        IN_LOBBY,
        IN_GAME
    };

    class GameController
    {

    public:
        /**
         * @brief Initialize the game controller
         *
         * @param gameWindow The main window of the game
         */
        static void init(GameWindow *game_window);

        /**
         * @brief Get all the necessary information to create a request message
         */
        static bool validInput(const wxString &input_server_address, const wxString &input_server_port,
                               const wxString &input_player_name, const wxString &input_game_name);

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
        static void playCard(const std::string &card_id);
        static void endActionPhase();
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

        /**
         * @brief refreshes Players when a new player joins the lobby
         */
        static void refreshPlayers(shared::JoinLobbyBroadcastMessage &msg);

        /**
         * @brief skips from the connection panel to the game panel for gui testing
         */
        static void skipToGamePanel();

        /**
         * @brief skips from the connection panel to the victory screen panel for gui testing
         */
        static void skipToVictoryScreenPanel();

    private:
        static void receiveActionOrderMessage(std::unique_ptr<shared::ActionOrderMessage> msg);
        static void receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> msg);
        static void receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg);
        static void receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg);
        static void receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg);
        static void receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> msg);

        static shared::PlayerBase::id_t getPlayerName();

        static GameWindow *_gameWindow;
        static ConnectionPanel *_connectionPanel;
        static MainGamePanel *_mainGamePanel;
        static LobbyPanel *_lobbyPanel;
        static VictoryScreenPanel *_victoryScreenPanel;

        static ClientNetworkManager *_clientNetworkManager;

        static ClientState _clientState;
        static std::unique_ptr<reduced::GameState> _gameState;
        static shared::PlayerBase::id_t _playerName;
        static std::string _gameName;
    };

} // namespace client
