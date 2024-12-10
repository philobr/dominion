#pragma once

#include <client_network_manager.h>
#include <gui_event_receiver.h>
#include <memory>
#include <windows/game_window.h>

#include <shared/game/game_state/reduced_game_state.h>
#include <shared/message_types.h>
#include <shared/player_result.h>

namespace client
{

    enum ClientState
    {
        LOGIN_SCREEN,
        CREATING_LOBBY,
        JOINING_LOBBY,
        IN_LOBBY,
        STARTING_GAME,
        IN_GAME,
        VICTORY_SCREEN,
    };

    class GameController
    {

    public:
        /**
         * @brief Initialize the game controller
         */
        GameController(GuiEventReceiver *event_receiver);

        /**
         * @brief Destroy the game controller
         */
        ~GameController() = default;

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
        void startGame(std::unordered_map<shared::CardBase::id_t, bool> selected_cards);

        void proceedToCardSelection();

        void buyCard(const std::string &card_id);
        void playCard(const std::string &card_id);
        void gainCardFromBoard(const std::string &card_id);
        void endActionPhase();
        void endTurn();
        void sendRequest(std::unique_ptr<shared::ClientToServerMessage> req);

        /**
         * @brief Receive a message from the server
         * @param msg The message received
         */
        void receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg);

        // TODO: This is for testing purposes only and will be removed later
        void skipToVictoryScreen();

        // TODO: This is for testing purposes only and will be removed later
        void skipToGamePanel();

        // TODO: This is for testing purposes only and will be removed later
        void skipToCardSelectionPanel();

        /**
         * @brief Show a message in the status bar
         * @param message The message to show
         */
        void showStatus(std::string message);

        /**
         * @brief Show an error message popup
         * @param title The title of the popup
         * @param message The content of the popup
         */
        void showError(std::string title, std::string message);

    private:
        void receiveActionOrderMessage(std::unique_ptr<shared::ActionOrderMessage> msg);
        void receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> msg);
        void receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg);
        void receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg);
        void receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg);
        void receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> msg);
        void receiveEndGameBroadcastMessage(std::unique_ptr<shared::EndGameBroadcastMessage> msg);

        void showLobbyScreen(std::vector<reduced::Player::id_t> players, bool is_game_master);
        void showGameScreen(std::unique_ptr<reduced::GameState> game_state);
        void showVictoryScreen(std::vector<shared::PlayerResult> results);
        void showCardSelectionScreen();
        void showGainFromBoard(std::unique_ptr<reduced::GameState> game_state, shared::GainFromBoardOrder order);

        bool isLobbyValid();

        std::unique_ptr<GuiEventReceiver> _guiEventReceiver;

        ClientNetworkManager *_clientNetworkManager;

        ClientState _clientState;

        size_t _numPlayers;
        shared::PlayerBase::id_t _playerName;
        std::string _gameName;
    };

} // namespace client
