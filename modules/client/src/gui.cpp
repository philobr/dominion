
#include <gui.h>
#include <shared/utils/logger.h>

namespace client
{
    Gui::Gui(GameWindow *game_window) :
        _gameWindow(game_window), _connectionPanel(new ConnectionPanel(game_window)),
        _mainGamePanel(new MainGamePanel(game_window)), _lobbyPanel(new LobbyPanel(game_window)),
        _victoryScreenPanel(new VictoryScreenPanel(game_window))
    {
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

    bool Gui::getConnectionForm(ConnectionForm &form)
    {
        form.host = _connectionPanel->getServerAddress().Trim().ToStdString();

        unsigned long port = 0;
        if ( !_connectionPanel->getServerPort().ToULong(&port) || port == 0 || port > 65535 ) {
            showError("Invalid port", "Port must be a number between 1 and 65535");
            return false;
        }
        form.port = port;

        form.player_name = _connectionPanel->getPlayerName().Trim().ToStdString();
        form.lobby_name = _connectionPanel->getGameName().Trim().ToStdString();

        return true;
    }

    void Gui::showError(const std::string &title, const std::string &message)
    {
        LOG(WARN) << title << ": " << message << std::endl;
        wxMessageBox(message, title, wxICON_ERROR);
    }

    void Gui::showStatus(const std::string &message) { _gameWindow->setStatus(message); }

    void Gui::showLobbyScreen(const std::vector<reduced::Player::id_t> &players, bool is_game_master)
    {
        LOG(INFO) << "Showing lobby screen";

        _gameWindow->showPanel(_lobbyPanel);
        if ( is_game_master ) {
            _lobbyPanel->makeGameMaster();
        }
        _lobbyPanel->refreshPlayers(players);
    }

    void Gui::showMainGameScreen() { _gameWindow->showPanel(_mainGamePanel); }

    void Gui::drawGameState(const reduced::GameState &game_state) { _mainGamePanel->drawGameState(game_state); }

    void Gui::showVictoryScreen()
    {
        _victoryScreenPanel->drawTestVictoryScreen();
        _gameWindow->showPanel(_victoryScreenPanel);
    }
} // namespace client
