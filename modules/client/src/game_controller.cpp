#include "game_controller.h"
#include <shared/utils/uuid_generator.h>


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

    bool GameController::validInput(const wxString &inputServerAddress, const wxString &inputServerPort,
                                    const wxString &inputPlayerName, const wxString &inputGameName)
    {

        // check that all values were provided
        if ( inputServerAddress.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's address");
            return false;
        }

        if ( inputServerPort.IsEmpty() ) {
            GameController::showError("Input error", "Please provide the server's port number");
            return false;
        }

        if ( inputPlayerName.IsEmpty() ) {
            GameController::showError("Input error", "Please enter your desired player name");
            return false;
        }

        if ( inputGameName.IsEmpty() ) {
            GameController::showError("Input error", "Please enter the game name");
            return false;
        }

        // convert port from wxString to uint16_t
        unsigned long portAsLong;
        if ( !inputServerPort.ToULong(&portAsLong) || portAsLong > 65535 ) {
            GameController::showError("Connection error", "Invalid port");
            return false;
        }

        return true;
    }

    void GameController::CreateLobby()
    {
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

            // detach gui from network atm
            if ( inputPlayerName == "testgigu" ) {

                GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
            }

            // send request to join game
            shared::CreateLobbyRequestMessage request(inputGameName.ToStdString(), uuid_generator::generate_uuid_v4(),
                                                      inputPlayerName.ToStdString());
            GameController::send_request(request.to_json());
        }
    }

    void GameController::JoinLobby()
    {
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
            shared::JoinLobbyRequestMessage request(inputGameName.ToStdString(), uuid_generator::generate_uuid_v4(),
                                                    inputPlayerName.ToStdString());
            GameController::send_request(request.to_json());
        }
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

        std::cerr << "Gamecontroller received message!" << std::endl;

        if ( shared::CreateLobbyResponseMessage *clrm =
                     dynamic_cast<shared::CreateLobbyResponseMessage *>(msg.get()) ) {
            // Show the lobby screen
            std::cerr << "Message is CreateLobbyResponse" << std::endl;
            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
            // TODO maybe add player_id to the ServerToClientMessage ?
            GameController::_lobbyPanel->AddPlayer(
                    GameController::_connectionPanel->getPlayerName().Trim().ToStdString());
            msg.release();
            std::cerr << "Done with Message" << std::endl;

        } else if ( shared::ResultResponseMessage *jlrm = dynamic_cast<shared::ResultResponseMessage *>(msg.get()) ) {
            std::cerr << "Message is ResultResponseMessage" << std::endl;
            // Show the lobby screen
            GameController::_gameWindow->showPanel(GameController::_lobbyPanel);
            msg.release();
            std::cerr << "Done with Message" << std::endl;
        } else if ( shared::JoinLobbyBroadcastMessage *jlbm =
                            dynamic_cast<shared::JoinLobbyBroadcastMessage *>(msg.get()) ) {
            std::cerr << "Message is JoinLobbyBroadcastMessage" << std::endl;
            GameController::RefreshPlayers(jlbm);
            msg.release();
            std::cerr << "Done with Message" << std::endl;
        } else {
            // This code should never be reached
            _ASSERT_FALSE(true, "Unknown message type");
        }
    }

    void GameController::RefreshPlayers(shared::JoinLobbyBroadcastMessage *msg)
    {
        // TODO: Clear the lobby panel
        for ( auto player : msg->players ) {
            GameController::_lobbyPanel->AddPlayer(player);
        }
    }
} // namespace client
