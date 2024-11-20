#include "game_controller.h"


namespace client
{
    // initialize static members
    GameWindow *GameController::_gameWindow = nullptr;
    ConnectionPanel *GameController::_connectionPanel = nullptr;
    MainGamePanel *GameController::_mainGamePanel = nullptr;
    LobbyPanel *GameController::_lobbyPanel = nullptr;


    void GameController::init(GameWindow *gameWindow)
    {

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
    }


    void GameController::connectToServer()
    {

        // get values form UI input fields
        wxString inputServerAddress = GameController::_connectionPanel->getServerAddress().Trim();
        wxString inputServerPort = GameController::_connectionPanel->getServerPort().Trim();
        wxString inputPlayerName = GameController::_connectionPanel->getPlayerName().Trim();
        wxString inputGameName = GameController::_connectionPanel->getGameName().Trim();

        // check that all values were provided
        if ( inputServerAddress.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's address");
            return;
        }
        if ( inputServerPort.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's port number");
            return;
        }
        if ( inputPlayerName.IsEmpty() ) {
            GameController::showError("Input error", "Please enter your desired player name");
            return;
        }
        if ( inputGameName.IsEmpty() ) {
            GameController::showError("Input error", "Please enter the game name");
            return;
        }

        // convert port from wxString to uint16_t
        unsigned long portAsLong;
        if ( !inputServerPort.ToULong(&portAsLong) || portAsLong > 65535 ) {
            GameController::showError("Connection error", "Invalid port");
            return;
        }

        // connect to the server
        _clientNetworkManager->init(inputServerAddress.ToStdString(), portAsLong);


        // detach gui from network atm
        if ( inputPlayerName.ToStdString() == "testgigu" ) {

            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
        }

        // send request to join game
        shared::CreateLobbyRequestMessage request(inputGameName.ToStdString(), "stuart", inputPlayerName.ToStdString());
        GameController::send_request(request.to_json());
    }

    void GameController::startGame()
    {
        // send request to start game
        //

        GameController::_gameWindow->showPanel(GameController::_mainGamePanel);
    }


    void GameController::showError(const std::string &title, const std::string &message)
    {
        wxMessageBox(message, title, wxICON_ERROR);
    }


    void GameController::showStatus(const std::string &message) { GameController::_gameWindow->setStatus(message); }

    void GameController::send_request(const std::string &req)
    {
        GameController::_clientNetworkManager->sendRequest(req);
    }


    void GameController::receive_message(std::unique_ptr<shared::ServerToClientMessage> msg)
    {
        // Show the lobby screen
        GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
        // TODO maybe add player_id to the ServerToClientMessage ?
        

        // TODO make compatible with different message types
        
        // in case of a JoinLobbyBroadcastMessage
        RefreshPlayers(std::move(msg));
    }

    void GameController::RefreshPlayers(std::unique_ptr<shared::ServerToClientMessage> msg)
    {
        shared::JoinLobbyBroadcastMessage *join_msg = dynamic_cast<shared::JoinLobbyBroadcastMessage *>(msg.get());
        delete GameController::_lobbyPanel;
        GameController::_lobbyPanel = new LobbyPanel(GameController::_gameWindow);

        for (auto player : join_msg->players)
        {
            GameController::_lobbyPanel->AddPlayer(player);
        }
    }

} // namespace client
