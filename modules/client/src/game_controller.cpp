#include "game_controller.h"
#include <shared/utils/logger.h>
#include <vector>
#include "shared/game/cards/card_base.h"
#include "shared/game/game_state/player_base.h"
#include "shared/message_types.h"


namespace client
{
    // initialize static members
    GameWindow *GameController::_gameWindow = nullptr;
    ConnectionPanel *GameController::_connectionPanel = nullptr;
    MainGamePanel *GameController::_mainGamePanel = nullptr;
    LobbyPanel *GameController::_lobbyPanel = nullptr;

    std::unique_ptr<reduced::GameState> GameController::_gameState = nullptr;
    std::string GameController::_gameName = "";
    shared::PlayerBase::id_t GameController::_playerName = "";

    void GameController::init(GameWindow *gameWindow)
    {

        LOG(INFO) << "GameController called in function init()";

        GameController::_gameWindow = gameWindow;

        // Set up main panels
        GameController::_connectionPanel = new ConnectionPanel(gameWindow);
        GameController::_mainGamePanel = new MainGamePanel(gameWindow);
        GameController::_lobbyPanel = new LobbyPanel(gameWindow);

        // Hide all panels
        GameController::_connectionPanel->Show(false);
        GameController::_mainGamePanel->Show(false);
        GameController::_lobbyPanel->Show(false);

        // Only show connection panel at the start of the game
        GameController::_gameWindow->showPanel(GameController::_connectionPanel);

        // Set status bar
        GameController::showStatus("Not connected");
        LOG(INFO) << "Done with GameController::init()";
    }

    bool GameController::validInput(const wxString &inputServerAddress, const wxString &inputServerPort,
                                    const wxString &inputPlayerName, const wxString &inputGameName)
    {
        LOG(INFO) << "GameController called in function validInput";
        // check that all values were provided
        if ( inputServerAddress.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's address");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        if ( inputServerPort.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's port number");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        if ( inputPlayerName.IsEmpty() ) {
            GameController::showError("Input error", "Please enter your desired player name");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        if ( inputGameName.IsEmpty() ) {
            GameController::showError("Input error", "Please enter the game name");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        // convert port from wxString to uint16_t
        unsigned long portAsLong;
        if ( !inputServerPort.ToULong(&portAsLong) || portAsLong > 65535 ) {
            GameController::showError("Connection error", "Invalid port");
            LOG(INFO) << "Done with GameController::validInput()";
            return false;
        }

        LOG(INFO) << "Done with GameController::validInput()";
        return true;
    }

    void GameController::createLobby()
    {
        LOG(INFO) << "GameController called in function CreateLobby()";
        // get values form UI input fields
        wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();
        wxString inputGameName = GameController::_connectionPanel->getGameName().Trim();

        if ( GameController::validInput(inputServerAddress, inputServerPort, inputPlayerName, inputGameName) ) {
            unsigned long portAsLong;
            inputServerPort.ToULong(&portAsLong);

            // connect to the server
            _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);

            // send request to join game
            shared::CreateLobbyRequestMessage request(inputGameName.ToStdString(), inputPlayerName.ToStdString());
            GameController::sendRequest(request.toJson());

            GameController::_gameName = inputGameName.ToStdString();
            GameController::_playerName = inputPlayerName.ToStdString();
        }
        LOG(INFO) << "Done with GameController::CreateLobby()";
    }

    void GameController::joinLobby()
    {
        LOG(INFO) << "GameController called in function JoinLobby()";
        // get values form UI input fields
        wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();
        wxString inputGameName = GameController::_connectionPanel->getGameName().Trim();
        if ( GameController::validInput(inputServerAddress, inputServerPort, inputPlayerName, inputGameName) ) {
            unsigned long portAsLong;
            inputServerPort.ToULong(&portAsLong);

            // connect to the server
            _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);

            // send request to join game
            shared::JoinLobbyRequestMessage request(inputGameName.ToStdString(), inputPlayerName.ToStdString());
            GameController::sendRequest(request.toJson());
        }
        LOG(INFO) << "Done with GameController::JoinLobby()";
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

    void GameController::playCard(unsigned int cardIndex)
    {
        LOG(INFO) << "Playing card at position" << cardIndex << std::endl;

        std::unique_ptr<shared::ActionDecision> decision(new shared::PlayActionCardDecision(cardIndex));

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
        LOG(INFO) << "Ending turn" << std::endl;

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
        LOG(INFO) << "Done with GameController::showError()";
    }


    void GameController::showStatus(const std::string &message) { GameController::_gameWindow->setStatus(message); }

    void GameController::sendRequest(const std::string &req)
    {
        LOG(INFO) << "GameController called in function send_request()";
        GameController::_clientNetworkManager->sendRequest(req);
        LOG(INFO) << "Done with GameController::send_request()";
    }

    void GameController::receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg)
    {
        GameController::_gameState = std::move(msg->game_state);
        GameController::_mainGamePanel->drawGameState(*GameController::_gameState);
    }

    void GameController::receiveMessage(std::unique_ptr<shared::ServerToClientMessage> msg)
    {
        LOG(INFO) << "Gamecontroller called in function receive_message()";

        shared::ServerToClientMessage &msgRef = *msg;
        if ( typeid(msgRef) == typeid(shared::CreateLobbyResponseMessage) ) {
            // Show the lobby screen
            LOG(INFO) << "Message is CreateLobbyResponse";
            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
            LOG(INFO) << "Switched panel";
            // TODO maybe add player_id to the ServerToClientMessage ?
            GameController::_lobbyPanel->addPlayer(
                    GameController::_connectionPanel->getPlayerName().Trim().ToStdString());
            LOG(INFO) << "Added Player";
        } else if ( typeid(msgRef) == typeid(shared::ResultResponseMessage) ) {
            LOG(INFO) << "Message is ResultResponseMessage";
            // Show the lobby screen
            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
        } else if ( typeid(msgRef) == typeid(shared::JoinLobbyBroadcastMessage) ) {
            std::unique_ptr<shared::JoinLobbyBroadcastMessage> jlbm(
                    static_cast<shared::JoinLobbyBroadcastMessage *>(msg.release()));
            LOG(INFO) << "Message is JoinLobbyBroadcastMessage";
            GameController::refreshPlayers(*jlbm);
        } else if ( typeid(msgRef) == typeid(shared::GameStateMessage) ) {
            std::unique_ptr<shared::GameStateMessage> gsm(static_cast<shared::GameStateMessage *>(msg.release()));
            LOG(INFO) << "Message is GameStateMessage";
            GameController::receiveGameStateMessage(std::move(gsm));
        } else {
            // This code should never be reached
            LOG(ERROR) << "Unknown message";
            _ASSERT_FALSE(true, "Unknown message type");
        }
    }

    void GameController::refreshPlayers(shared::JoinLobbyBroadcastMessage &msg)
    {
        LOG(INFO) << "Refreshing Players";
        GameController::_lobbyPanel->refreshPlayers(msg.players);
        /*
        for ( auto player : msg->players ) {
            GameController::_lobbyPanel->AddPlayer(player);
        }*/
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
} // namespace client
