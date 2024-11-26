#include "game_controller.h"
#include <shared/utils/logger.h>


namespace client
{
    // initialize static members
    GameWindow *GameController::_gameWindow = nullptr;
    ConnectionPanel *GameController::_connectionPanel = nullptr;
    MainGamePanel *GameController::_mainGamePanel = nullptr;
    LobbyPanel *GameController::_lobbyPanel = nullptr;


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

    void GameController::CreateLobby()
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
            GameController::send_request(request.to_json());
        }
        LOG(INFO) << "Done with GameController::CreateLobby()";
    }

    void GameController::JoinLobby()
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
            GameController::send_request(request.to_json());
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
        // send request to buy card


        // For testing delete later
        LOG(DEBUG) << "Tried to buy card " << card_id << std::endl;
        showStatus("Tried to buy card " + card_id);
    }

    void GameController::playCard(const std::string &card_id)
    {
        // send request to play card

        // For testing delete later
        LOG(DEBUG) << "Tried to play card " << card_id << std::endl;
        showStatus("Tried to play card " + card_id);
    }


    void GameController::showError(const std::string &title, const std::string &message)
    {
        LOG(WARN) << title << ": " << message << std::endl;
        wxMessageBox(message, title, wxICON_ERROR);
        LOG(INFO) << "Done with GameController::showError()";
    }


    void GameController::showStatus(const std::string &message) { GameController::_gameWindow->setStatus(message); }

    void GameController::send_request(const std::string &req)
    {
        LOG(INFO) << "GameController called in function send_request()";
        GameController::_clientNetworkManager->sendRequest(req);
        LOG(INFO) << "Done with GameController::send_request()";
    }


    void GameController::receive_message(std::unique_ptr<shared::ServerToClientMessage> msg)
    {

        LOG(INFO) << "Gamecontroller called in function receive_message()";

        if ( typeid(*msg) == typeid(shared::CreateLobbyResponseMessage) ) {
            // Show the lobby screen
            LOG(INFO) << "Message is CreateLobbyResponse";
            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
            LOG(INFO) << "Switched panel";
            // TODO maybe add player_id to the ServerToClientMessage ?
            GameController::_lobbyPanel->AddPlayer(
                    GameController::_connectionPanel->getPlayerName().Trim().ToStdString());
            LOG(INFO) << "Added Player";
        } else if ( typeid(*msg) == typeid(shared::ResultResponseMessage) ) {
            LOG(INFO) << "Message is ResultResponseMessage";
            // Show the lobby screen
            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
        } else if ( typeid(*msg) == typeid(shared::JoinLobbyBroadcastMessage) ) {
            std::unique_ptr<shared::JoinLobbyBroadcastMessage> jlbm(
                    static_cast<shared::JoinLobbyBroadcastMessage *>(msg.get()));
            LOG(INFO) << "Message is JoinLobbyBroadcastMessage";
            GameController::RefreshPlayers(*jlbm);
        } else {
            // This code should never be reached
            LOG(ERROR) << "Unknown message";
            _ASSERT_FALSE(true, "Unknown message type");
        }
    }

    void GameController::RefreshPlayers(shared::JoinLobbyBroadcastMessage &msg)
    {
        LOG(INFO) << "Refreshing Players";
        GameController::_lobbyPanel->refreshPlayers(msg.players);
        /*
        for ( auto player : msg->players ) {
            GameController::_lobbyPanel->AddPlayer(player);
        }*/
        LOG(INFO) << "Added new players";
    }
} // namespace client
