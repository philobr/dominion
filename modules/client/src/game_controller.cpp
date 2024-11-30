#include "game_controller.h"
#include <memory>
#include <shared/utils/logger.h>
#include <vector>
#include "shared/game/cards/card_base.h"
#include "shared/game/game_state/player_base.h"
#include "shared/message_types.h"

using namespace shared;

namespace client
{
    GameController::GameController(GameWindow *game_window) :
        _gameWindow(game_window), _connectionPanel(new ConnectionPanel(game_window)),
        _mainGamePanel(new MainGamePanel(game_window)), _lobbyPanel(new LobbyPanel(game_window)),
        _victoryScreenPanel(new VictoryScreenPanel(game_window)), _clientState(ClientState::LOGIN_SCREEN)
    {
        LOG(INFO) << "Initializing GameController";

        // Hide all panels
        _connectionPanel->Show(false);
        _mainGamePanel->Show(false);
        _lobbyPanel->Show(false);
        _victoryScreenPanel->Show(false);

        // Only show connection panel at the start of the game
        _gameWindow->showPanel(_connectionPanel);

        // Set status bar
        showStatus("Not connected");
    }

    bool GameController::validInput(const wxString &input_server_address, const wxString &input_server_port,
                                    const wxString &input_player_name, const wxString &input_game_name)
    {
        // check that all values were provided
        if ( input_server_address.IsEmpty() ) {
            showError("Input error", "Please provide the server's address");
            return false;
        }

        if ( input_server_port.IsEmpty() ) {
            showError("Input error", "Please provide the server's port number");
            return false;
        }

        if ( input_player_name.IsEmpty() ) {
            showError("Input error", "Please enter your desired player name");
            return false;
        }

        if ( input_game_name.IsEmpty() ) {
            showError("Input error", "Please enter the game name");
            return false;
        }

        // convert port from wxString to uint16_t
        unsigned long portAsLong;
        if ( !input_server_port.ToULong(&portAsLong) || portAsLong > 65535 ) {
            showError("Connection error", "Invalid port");
            return false;
        }

        return true;
    }

    void GameController::createLobby()
    {
        if ( _clientState != ClientState::LOGIN_SCREEN ) {
            showError("Error", "Tried to create lobby while not in login screen");
            return;
        }

        // get values form UI input fields
        wxString inputServerAddress = _connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = _connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = _connectionPanel->getPlayerName().Trim();
        wxString inputGameName = _connectionPanel->getGameName().Trim();

        LOG(DEBUG) << "Creating lobby " << inputGameName;

        if ( validInput(inputServerAddress, inputServerPort, inputPlayerName, inputGameName) ) {
            unsigned long portAsLong;
            inputServerPort.ToULong(&portAsLong);

            // connect to the server
            _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);
            if ( _clientNetworkManager->failedToConnect() ) {
                _clientState = ClientState::LOGIN_SCREEN;
                LOG(INFO) << "Reverted to ClientState::LOGIN_SCREEN";
            } else {

                // send request to join game
                shared::CreateLobbyRequestMessage request(inputGameName.ToStdString(), inputPlayerName.ToStdString());
                sendRequest(request.toJson());

                _gameName = inputGameName.ToStdString();
                _playerName = inputPlayerName.ToStdString();
                _clientState = ClientState::CREATING_LOBBY;
            }
        }
    }

    void GameController::joinLobby()
    {
        if ( _clientState != ClientState::LOGIN_SCREEN ) {
            showError("Error", "Tried to join lobby while not in login screen");
            return;
        }

        // get values form UI input fields
        wxString inputServerAddress = _connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = _connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = _connectionPanel->getPlayerName().Trim();
        wxString inputGameName = _connectionPanel->getGameName().Trim();

        LOG(DEBUG) << "Joining lobby " << inputGameName;

        if ( validInput(inputServerAddress, inputServerPort, inputPlayerName, inputGameName) ) {
            unsigned long portAsLong;
            inputServerPort.ToULong(&portAsLong);

            // connect to the server
            _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);

            // send request to join game
            shared::JoinLobbyRequestMessage request(inputGameName.ToStdString(), inputPlayerName.ToStdString());
            sendRequest(request.toJson());

            _gameName = inputGameName.ToStdString();
            _playerName = inputPlayerName.ToStdString();
            _clientState = ClientState::JOINING_LOBBY;
        }
    }

    void GameController::startGame()
    {
        LOG(DEBUG) << "Starting game";
        std::vector<shared::CardBase::id_t> selectedCards{"Estate",       "Smithy",      "Village",      "Laboratory",
                                                          "Festival",     "Market",      "Placeholder1", "Placeholder2",
                                                          "Placeholder3", "Placeholder4"};
        shared::StartGameRequestMessage msg = shared::StartGameRequestMessage(_gameName, _playerName, selectedCards);
        sendRequest(msg.toJson());
    }


    void GameController::buyCard(const std::string &card_id)
    {
        LOG(DEBUG) << "Buying card " << card_id << std::endl;

        std::unique_ptr<shared::ActionDecision> decision(new shared::BuyCardDecision(card_id));

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, getPlayerName(), std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::playCard(const std::string &card_id)
    {
        LOG(DEBUG) << "Playing card " << card_id;

        std::unique_ptr<shared::ActionDecision> decision(new shared::PlayActionCardDecision(card_id));

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, getPlayerName(), std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::endActionPhase()
    {
        LOG(INFO) << "Ending action phase";

        std::unique_ptr<shared::ActionDecision> decision(new shared::EndActionPhaseDecision());

        // TODO (#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, getPlayerName(), std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::endTurn()
    {
        LOG(DEBUG) << "Ending turn";

        std::unique_ptr<shared::ActionDecision> decision(new shared::EndTurnDecision());

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(_gameName, getPlayerName(), std::move(decision),
                                                                in_response_to);

        _clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::showError(const std::string &title, const std::string &message)
    {
        LOG(WARN) << title << ": " << message << std::endl;
        wxMessageBox(message, title, wxICON_ERROR);
    }

    void GameController::showStatus(const std::string &message) { _gameWindow->setStatus(message); }

    void GameController::sendRequest(const std::string &req) { _clientNetworkManager->sendRequest(req); }

    void GameController::receiveActionOrderMessage(std::unique_ptr<shared::ActionOrderMessage> /*msg*/)
    {
        // TODO(#125) This is not implemented, and will probably be removed with #125
        LOG(WARN) << "Received ActionOrderMessage, but this does not do anything yet";
    }

    void GameController::receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> /*msg*/)
    {
        _gameWindow->showPanel(_lobbyPanel);
        // TODO maybe add player_id to the ServerToClientMessage ?
        _lobbyPanel->makeGameMaster();
        _lobbyPanel->addPlayer(_connectionPanel->getPlayerName().Trim().ToStdString());
    }

    void GameController::receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg)
    {
        LOG(DEBUG) << "Player joined lobby: " << msg->players.back();
        std::unique_ptr<shared::JoinLobbyBroadcastMessage> jlbm(
                static_cast<shared::JoinLobbyBroadcastMessage *>(msg.release()));

        refreshPlayers(*jlbm);
    }

    void GameController::receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg)
    {
        switch ( _clientState ) {
            case ClientState::LOGIN_SCREEN:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in login screen";
                break;
            case ClientState::JOINING_LOBBY:
                if ( msg->success ) {
                    LOG(DEBUG) << "Successfully joined lobby";
                    _gameWindow->showPanel(_lobbyPanel);
                    _clientState = ClientState::IN_LOBBY;
                } else {
                    LOG(DEBUG) << "Failed to join lobby";
                    if ( msg->additional_information.has_value() ) {
                        showError("Failed to join lobby", msg->additional_information.value());
                    } else {
                        showError("Failed to join lobby", "");
                    }
                    LOG(INFO) << "Returning to login screen";
                    _clientState = ClientState::LOGIN_SCREEN;
                }
                break;
            case ClientState::CREATING_LOBBY:
                if ( msg->success ) {
                    LOG(DEBUG) << "Successfully created lobby";
                    _gameWindow->showPanel(_lobbyPanel);
                    _clientState = ClientState::IN_LOBBY;
                } else {
                    LOG(DEBUG) << "Failed to create lobby";
                    if ( msg->additional_information.has_value() ) {
                        showError("Failed to create lobby", msg->additional_information.value());
                    } else {
                        showError("Failed to create lobby", "");
                    }
                    LOG(INFO) << "Returning to login screen";
                    _clientState = ClientState::LOGIN_SCREEN;
                }
                break;
            case ClientState::IN_LOBBY:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in lobby";
                break;
            case ClientState::IN_GAME:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in running game";
                break;
            default:
                LOG(WARN) << "Received ResultResponseMessage, but client is in unknown state";
                break;
        }
    }

    void GameController::receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg)
    {
        LOG(INFO) << "Received GameStateMessage, updating game state";
        _gameState = std::move(msg->game_state);
        _mainGamePanel->drawGameState(*_gameState);
    }

    void GameController::receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> /*msg*/)
    {
        LOG(DEBUG) << "Starting game";
        _gameWindow->showPanel(_mainGamePanel);
    }

    void GameController::receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg)
    {
// NOLINTBEGIN(bugprone-macro-parentheses)
#define HANDLE_MESSAGE(type)                                                                                           \
    if ( typeid(msgRef) == typeid(type) ) {                                                                            \
        LOG(INFO) << "Received message of type " << #type;                                                             \
        std::unique_ptr<type> casted(static_cast<type *>(msg.release()));                                              \
        receive##type(std::move(casted));                                                                              \
        return;                                                                                                        \
    }
        // NOLINTEND(bugprone-macro-parentheses)
        ServerToClientMessage &msgRef = *msg;
        HANDLE_MESSAGE(ActionOrderMessage);
        HANDLE_MESSAGE(CreateLobbyResponseMessage);
        HANDLE_MESSAGE(JoinLobbyBroadcastMessage);
        HANDLE_MESSAGE(ResultResponseMessage);
        HANDLE_MESSAGE(GameStateMessage);
        HANDLE_MESSAGE(StartGameBroadcastMessage);
#undef HANDLE_MESSAGE

        LOG(ERROR) << "Unknown message type";
        throw exception::UnreachableCode("Unknown message type");
    }

    void GameController::refreshPlayers(shared::JoinLobbyBroadcastMessage &msg)
    {
        LOG(INFO) << "Refreshing Players on lobby panel";
        _lobbyPanel->refreshPlayers(msg.players);
    }

    shared::PlayerBase::id_t GameController::getPlayerName()
    {
        if ( _gameState == nullptr ) {
            LOG(ERROR) << "GameController::getPlayerName called without a game state";
            throw exception::UnreachableCode(
                    "GameController::getPlayerName should never be called without a game state");
        }
        return _gameState->reduced_player->getId();
    }

    void GameController::skipToGamePanel() { _gameWindow->showPanel(_mainGamePanel); }
    void GameController::skipToVictoryScreenPanel()
    {
        _victoryScreenPanel->drawTestVictoryScreen();
        _gameWindow->showPanel(_victoryScreenPanel);
    }
} // namespace client
