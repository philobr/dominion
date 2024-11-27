#include "game_controller.h"
#include <shared/utils/logger.h>

using namespace shared;

namespace client
{
    // initialize static members
    GameWindow *GameController::_gameWindow = nullptr;
    ConnectionPanel *GameController::_connectionPanel = nullptr;
    MainGamePanel *GameController::_mainGamePanel = nullptr;
    LobbyPanel *GameController::_lobbyPanel = nullptr;

    std::unique_ptr<reduced::GameState> GameController::_gameState = nullptr;
    std::string GameController::_gameName = "";

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
        GameController::_gameWindow->showPanel(GameController::_mainGamePanel);
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

    void GameController::receiveCreateLobbyResponseMessage(std::unique_ptr<shared::CreateLobbyResponseMessage> /*msg*/)
    {
        GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
        // TODO maybe add player_id to the ServerToClientMessage ?
        GameController::_lobbyPanel->addPlayer(GameController::_connectionPanel->getPlayerName().Trim().ToStdString());
    }

    void GameController::receiveJoinLobbyBroadcastMessage(std::unique_ptr<shared::JoinLobbyBroadcastMessage> msg)
    {
        std::unique_ptr<shared::JoinLobbyBroadcastMessage> jlbm(
                static_cast<shared::JoinLobbyBroadcastMessage *>(msg.release()));
        GameController::refreshPlayers(*jlbm);
    }

    void GameController::receiveResultResponseMessage(std::unique_ptr<shared::ResultResponseMessage> /*msg*/)
    {
        GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
    }

    void GameController::receiveGameStateMessage(std::unique_ptr<shared::GameStateMessage> msg)
    {
        GameController::_gameState = std::move(msg->game_state);
        GameController::_mainGamePanel->drawGameState(*GameController::_gameState);
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
        HANDLE_MESSAGE(CreateLobbyResponseMessage);
        HANDLE_MESSAGE(JoinLobbyBroadcastMessage);
        HANDLE_MESSAGE(ResultResponseMessage);
        HANDLE_MESSAGE(GameStateMessage);
#undef HANDLE_MESSAGE

        LOG(ERROR) << "Unknown message type";
        throw exception::UnreachableCode("Unknown message type");
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
