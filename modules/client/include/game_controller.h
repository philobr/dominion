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
         * @param gameWindow The main window of the game
         */
        GameController(GameWindow *game_window);

        /**
         * @brief Destroy the game controller
         */
        ~GameController();

        /**
         * @brief Get all the necessary information to create a request message
         */
        bool validInput(const wxString &input_server_address, const wxString &input_server_port,
                        const wxString &input_player_name, const wxString &input_game_name);

        /**
         * @brief Show the connection panel and send create lobby request
         */
        void createLobby();

        /**
         * @brief Join a Lobby
         */
        void joinLobby();

        /**
         * @brief Show the main game panel
         *
         */
        void startGame();

        void buyCard(const std::string &card_id);
        void playCard(const std::string &card_id);
        void endActionPhase();
        void endTurn();
        void sendRequest(const std::string &req);

        /**
         * @brief Receive a message from the server
         * @param msg The message received
         */
        void receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg);

        /**
         * @brief display an error message
         *
         */
        void showError(const std::string &title, const std::string &message);

        /**
         * @brief display a status message in the status bar
         *
         */
        void showStatus(const std::string &message);

        /**
         * @brief refreshes Players when a new player joins the lobby
         */
        void refreshPlayers(shared::JoinLobbyBroadcastMessage &msg);

        /**
         * @brief skips from the connection panel to the game panel for gui testing
         */
        void skipToGamePanel();

        /**
         * @brief skips from the connection panel to the victory screen panel for gui testing
         */
        void skipToVictoryScreenPanel();

    private:
        void receiveActionOrderMessage(std::unique_ptr<shared::ActionOrderMessage> msg);
        void receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> msg);
        void receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg);
        void receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg);
        void receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg);
        void receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> msg);

        shared::PlayerBase::id_t getPlayerName();

        GameWindow *_gameWindow;
        ConnectionPanel *_connectionPanel;
        MainGamePanel *_mainGamePanel;
        LobbyPanel *_lobbyPanel;
        VictoryScreenPanel *_victoryScreenPanel;

        ClientNetworkManager *_clientNetworkManager;

        ClientState _clientState;
        std::unique_ptr<reduced::GameState> _gameState;
        shared::PlayerBase::id_t _playerName;
        std::string _gameName;
    };

} // namespace client
