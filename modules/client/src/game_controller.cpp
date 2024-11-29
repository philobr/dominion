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
    // initialize static members
    GameWindow *GameController::_gameWindow = nullptr;
    ConnectionPanel *GameController::_connectionPanel = nullptr;
    MainGamePanel *GameController::_mainGamePanel = nullptr;
    LobbyPanel *GameController::_lobbyPanel = nullptr;
    VictoryScreenPanel *GameController::_victoryScreenPanel = nullptr;

    ClientState GameController::_clientState = ClientState::LOGIN_SCREEN;
    std::unique_ptr<reduced::GameState> GameController::_gameState = nullptr;
    std::string GameController::_gameName = "";
    shared::PlayerBase::id_t GameController::_playerName = "";

    void GameController::init(GameWindow *game_window)
    {

        LOG(INFO) << "GameController called in function init()";

        GameController::_gameWindow = game_window;

        // Set up main panels
        GameController::_connectionPanel = new ConnectionPanel(_gameWindow);
        GameController::_mainGamePanel = new MainGamePanel(_gameWindow);
        GameController::_lobbyPanel = new LobbyPanel(_gameWindow);
        GameController::_victoryScreenPanel = new VictoryScreenPanel(_gameWindow);

        // Hide all panels
        GameController::_connectionPanel->Show(false);
        GameController::_mainGamePanel->Show(false);
        GameController::_lobbyPanel->Show(false);
        GameController::_victoryScreenPanel->Show(false);

        // Only show connection panel at the start of the game
        GameController::_gameWindow->showPanel(GameController::_connectionPanel);

        // Set status bar
        GameController::showStatus("Not connected");
        LOG(INFO) << "Done with GameController::init()";
    }

    bool GameController::validInput(const wxString &input_server_address, const wxString &input_server_port,
                                    const wxString &input_player_name, const wxString &input_game_name)
    {
        LOG(INFO) << "GameController called in function validInput";
        // check that all values were provided
        if ( input_server_address.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's address");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        if ( input_server_port.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's port number");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        if ( input_player_name.IsEmpty() ) {
            GameController::showError("Input error", "Please enter your desired player name");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        if ( input_game_name.IsEmpty() ) {
            GameController::showError("Input error", "Please enter the game name");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        // convert port from wxString to uint16_t
        unsigned long portAsLong;
        if ( !input_server_port.ToULong(&portAsLong) || portAsLong > 65535 ) {
            GameController::showError("Connection error", "Invalid port");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        LOG(INFO) << "Done with GameController::validInput()";
        return true;
    }

    void GameController::createLobby()
    {
        if ( GameController::_clientState != ClientState::LOGIN_SCREEN ) {
            GameController::showError("Error", "Tried to create lobby while not in login screen");
            return;
        }

        // get values form UI input fields
        wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();
        wxString inputGameName = GameController::_connectionPanel->getGameName().Trim();

        LOG(DEBUG) << "Creating lobby " << inputGameName;

        if ( GameController::validInput(inputServerAddress, inputServerPort, inputPlayerName, inputGameName) ) {
            unsigned long portAsLong;
            inputServerPort.ToULong(&portAsLong);

            // connect to the server
            _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);
            if ( _clientNetworkManager->failedToConnect() ) {
                GameController::_clientState = ClientState::LOGIN_SCREEN;
                LOG(INFO) << "Reverted to ClientState::LOGIN_SCREEN";
            } else {

                // send request to join game
                shared::CreateLobbyRequestMessage request(inputGameName.ToStdString(), inputPlayerName.ToStdString());
                GameController::sendRequest(request.toJson());

                GameController::_gameName = inputGameName.ToStdString();
                GameController::_playerName = inputPlayerName.ToStdString();
                GameController::_clientState = ClientState::CREATING_LOBBY;
            }
        }
    }

    void GameController::joinLobby()
    {
        if ( GameController::_clientState != ClientState::LOGIN_SCREEN ) {
            GameController::showError("Error", "Tried to join lobby while not in login screen");
            return;
        }

        // get values form UI input fields
        wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();
        wxString inputGameName = GameController::_connectionPanel->getGameName().Trim();

        LOG(DEBUG) << "Joining lobby " << inputGameName;

        if ( GameController::validInput(inputServerAddress, inputServerPort, inputPlayerName, inputGameName) ) {
            unsigned long portAsLong;
            inputServerPort.ToULong(&portAsLong);

            // connect to the server
            _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);

            // send request to join game
            shared::JoinLobbyRequestMessage request(inputGameName.ToStdString(), inputPlayerName.ToStdString());
            GameController::sendRequest(request.toJson());

            GameController::_gameName = inputGameName.ToStdString();
            GameController::_playerName = inputPlayerName.ToStdString();
            GameController::_clientState = ClientState::JOINING_LOBBY;
        }
    }

    void GameController::startGame()
    {
        // send request to start game
        LOG(INFO) << "GameController called in function startGame()";
        std::vector<shared::CardBase::id_t> selectedCards{"Estate",       "Smithy",      "Village",      "Laboratory",
                                                          "Festival",     "Market",      "Placeholder1", "Placeholder2",
                                                          "Placeholder3", "Placeholder4"};
        shared::StartGameRequestMessage msg =
                shared::StartGameRequestMessage(GameController::_gameName, GameController::_playerName, selectedCards);
        GameController::sendRequest(msg.toJson());
        LOG(INFO) << "Done with GameController::startGame()";
    }


    void GameController::buyCard(const std::string &card_id)
    {
        LOG(INFO) << "Buying card " << card_id << std::endl;

        std::unique_ptr<shared::ActionDecision> decision(new shared::BuyCardDecision(card_id));

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(GameController::_gameName,
                                                                GameController::getPlayerName(), std::move(decision),
                                                                in_response_to);

        GameController::_clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::playCard(const std::string &card_id)
    {
        LOG(INFO) << "Playing card " << card_id;

        std::unique_ptr<shared::ActionDecision> decision(new shared::PlayActionCardDecision(card_id));

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(GameController::_gameName,
                                                                GameController::getPlayerName(), std::move(decision),
                                                                in_response_to);

        GameController::_clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::endTurn()
    {
        LOG(INFO) << "Ending turn";

        std::unique_ptr<shared::ActionDecision> decision(new shared::EndTurnDecision());

        // TODO(#120) Implement in_response_to
        std::optional<std::string> in_response_to = std::nullopt;

        std::unique_ptr<shared::ActionDecisionMessage> action_decision_message =
                std::make_unique<shared::ActionDecisionMessage>(GameController::_gameName,
                                                                GameController::getPlayerName(), std::move(decision),
                                                                in_response_to);

        GameController::_clientNetworkManager->sendRequest(action_decision_message->toJson());
    }

    void GameController::showError(const std::string &title, const std::string &message)
    {
        LOG(WARN) << title << ": " << message << std::endl;
        wxMessageBox(message, title, wxICON_ERROR);
    }

    void GameController::showStatus(const std::string &message) { GameController::_gameWindow->setStatus(message); }

    void GameController::sendRequest(const std::string &req)
    {
        GameController::_clientNetworkManager->sendRequest(req);
    }

    void GameController::receiveActionOrderMessage(std::unique_ptr<shared::ActionOrderMessage> /*msg*/)
    {
        // TODO(#125) This is not implemented, and will probably be removed with #125
        LOG(WARN) << "Received ActionOrderMessage, but this does not do anything yet";
    }

    void GameController::receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> /*msg*/)
    {
        LOG(INFO) << "GameController called in function receiveCreateLobbyResponseMessage()";
        GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
        // TODO maybe add player_id to the ServerToClientMessage ?
        GameController::_lobbyPanel->makeGameMaster();
        GameController::_lobbyPanel->addPlayer(GameController::_connectionPanel->getPlayerName().Trim().ToStdString());
        LOG(INFO) << "Done with receiveCreateLobbyResponseMessage()";
    }

    void GameController::receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg)
    {
        LOG(DEBUG) << "Player joined lobby: " << msg->players.back();
        std::unique_ptr<shared::JoinLobbyBroadcastMessage> jlbm(
                static_cast<shared::JoinLobbyBroadcastMessage *>(msg.release()));

        GameController::refreshPlayers(*jlbm);
        LOG(INFO) << "Done with receiveJoinLobbyBroadcastMessage()";
    }

    void GameController::receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> msg)
    {
        switch ( GameController::_clientState ) {
            case ClientState::LOGIN_SCREEN:
                LOG(WARN) << "Received unexpected ResultResponseMessage while in login screen";
                break;
            case ClientState::JOINING_LOBBY:
                if ( msg->success ) {
                    LOG(DEBUG) << "Successfully joined lobby";
                    GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
                    GameController::_clientState = ClientState::IN_LOBBY;
                } else {
                    LOG(DEBUG) << "Failed to join lobby";
                    if ( msg->additional_information.has_value() ) {
                        GameController::showError("Failed to join lobby", msg->additional_information.value());
                    } else {
                        GameController::showError("Failed to join lobby", "");
                    }
                    LOG(INFO) << "Returning to login screen";
                    GameController::_clientState = ClientState::LOGIN_SCREEN;
                }
                break;
            case ClientState::CREATING_LOBBY:
                if ( msg->success ) {
                    LOG(DEBUG) << "Successfully created lobby";
                    GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
                    GameController::_clientState = ClientState::IN_LOBBY;
                } else {
                    LOG(DEBUG) << "Failed to create lobby";
                    if ( msg->additional_information.has_value() ) {
                        GameController::showError("Failed to create lobby", msg->additional_information.value());
                    } else {
                        GameController::showError("Failed to create lobby", "");
                    }
                    LOG(INFO) << "Returning to login screen";
                    GameController::_clientState = ClientState::LOGIN_SCREEN;
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
        LOG(INFO) << "GameController called in function receiveGameStateMessage()";
        GameController::_gameState = std::move(msg->game_state);
        LOG(INFO) << "Updated GameState";
        GameController::_mainGamePanel->drawGameState(*GameController::_gameState);
        LOG(INFO) << "Done with receiveGameStateMessage()";
    }

    void GameController::receiveStartGameBroadcastMessage(std::unique_ptr<shared::StartGameBroadcastMessage> /*msg*/)
    {
        LOG(INFO) << "GameController called in function receiveStartGameBroadcastMessage()";
        GameController::_gameWindow->showPanel(GameController::_mainGamePanel);
        LOG(INFO) << "Done with receiveStartGameBroadcastMessage()";
    }

    void GameController::receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg)
    {
// NOLINTBEGIN(bugprone-macro-parentheses)
#define HANDLE_MESSAGE(type)                                                                                           \
    if ( typeid(msgRef) == typeid(type) ) {                                                                            \
        LOG(INFO) << "Received message of type " << #type;                                                             \
        std::unique_ptr<type> casted(static_cast<type *>(msg.release()));                                              \
        GameController::receive##type(std::move(casted));                                                              \
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
        LOG(INFO) << "Refreshing Players";
        GameController::_lobbyPanel->refreshPlayers(msg.players);
        LOG(INFO) << "Added new players";
    }

    shared::PlayerBase::id_t GameController::getPlayerName()
    {
        if ( GameController::_gameState == nullptr ) {
            LOG(ERROR) << "GameController::getPlayerName called without a game state";
            throw exception::UnreachableCode(
                    "GameController::getPlayerName should never be called without a game state");
        }
        return GameController::_gameState->reduced_player->getId();
    }

    void GameController::skipToGamePanel() { GameController::_gameWindow->showPanel(GameController::_mainGamePanel); }
    void GameController::skipToVictoryScreenPanel()
    {
        GameController::_victoryScreenPanel->drawTestVictoryScreen();
        GameController::_gameWindow->showPanel(GameController::_victoryScreenPanel);
    }
} // namespace client
