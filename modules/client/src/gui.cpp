
#include <gui.h>
#include <shared/utils/logger.h>

/**
 * Macro to check if the current thread is the main thread.
 * If not, a warning is logged.
 *
 * Every GUI function should be called from the main thread.
 * This macro is used to check if this is the case.
 */
#define ONLY_MAIN_THREAD()                                                                                             \
    do {                                                                                                               \
        if ( !wxThread::IsMain() ) {                                                                                   \
            LOG(WARN) << "Warning: GUI code not running in main thread" << std::endl;                                  \
        }                                                                                                              \
    } while ( 0 )

namespace client
{
    Gui::Gui(GameWindow *game_window) :
        _gameWindow(game_window), _connectionPanel(new ConnectionPanel(game_window)),
        _mainGamePanel(new MainGamePanel(game_window)), _lobbyPanel(new LobbyPanel(game_window)),
        _victoryScreenPanel(new VictoryScreenPanel(game_window)),
        _cardSelectionPanel(new CardSelectionPanel(game_window))
    {
        ONLY_MAIN_THREAD();

        // Hide all panels
        _connectionPanel->Show(false);
        _mainGamePanel->Show(false);
        _lobbyPanel->Show(false);
        _victoryScreenPanel->Show(false);
        _cardSelectionPanel->Show(false);

        // Only show connection panel at the start of the game
        _gameWindow->showPanel(_connectionPanel);

        // Set status bar
        showStatus("Not connected");
    }

    bool Gui::getConnectionForm(ConnectionForm &form)
    {
        ONLY_MAIN_THREAD();

        form.host = _connectionPanel->getServerAddress().Trim().ToStdString();

        if ( form.host.empty() ) {
            showError("Invalid host", "Host must not be empty");
            return false;
        }

        unsigned long port = 0;
        if ( !_connectionPanel->getServerPort().ToULong(&port) || port == 0 || port > 65535 ) {
            showError("Invalid port", "Port must be a number between 1 and 65535");
            return false;
        }
        form.port = port;

        form.player_name = _connectionPanel->getPlayerName().Trim().ToStdString();

        if ( form.player_name.empty() ) {
            showError("Invalid player name", "Player name must not be empty");
            return false;
        }

        form.lobby_name = _connectionPanel->getGameName().Trim().ToStdString();

        if ( form.lobby_name.empty() ) {
            showError("Invalid lobby name", "Lobby name must not be empty");
            return false;
        }

        return true;
    }

    void Gui::showError(const std::string &title, const std::string &message)
    {
        ONLY_MAIN_THREAD();

        LOG(WARN) << title << ": " << message << std::endl;
        wxMessageBox(message, title, wxICON_ERROR);
    }

    void Gui::showStatus(const std::string &message)
    {
        ONLY_MAIN_THREAD();

        _gameWindow->setStatus(message);
    }

    void Gui::showLobbyScreen(const std::vector<reduced::Player::id_t> &players, bool is_game_master)
    {
        ONLY_MAIN_THREAD();

        LOG(INFO) << "Showing lobby screen";

        _gameWindow->showPanel(_lobbyPanel);
        if ( is_game_master ) {
            _lobbyPanel->makeGameMaster();
        }
        _lobbyPanel->refreshPlayers(players);
    }

    void Gui::showGameScreen(std::shared_ptr<reduced::GameState> game_state)
    {
        ONLY_MAIN_THREAD();

        LOG(INFO) << "Showing game screen";

        if ( game_state == nullptr ) {
            LOG(WARN) << "Game state is null" << std::endl;
        } else {
            _mainGamePanel->drawGameState(*game_state);
        }
        _gameWindow->showPanel(_mainGamePanel);
    }

    void Gui::showVictoryScreen(const std::vector<shared::PlayerResult> &results)
    {
        ONLY_MAIN_THREAD();

        LOG(INFO) << "Showing victory screen";

        _victoryScreenPanel->drawVictoryScreen(results);
        _gameWindow->showPanel(_victoryScreenPanel);
    }

    void Gui::showCardSelectionScreen()
    {
        ONLY_MAIN_THREAD();

        LOG(INFO) << "Showing card selection screen";

        _gameWindow->showPanel(_cardSelectionPanel);
    }

    void Gui::showGainFromBoardScreen(std::shared_ptr<reduced::GameState> game_state, shared::GainFromBoardOrder order)
    {
        ONLY_MAIN_THREAD();

        LOG(INFO) << "Showing gain from board overlay";

        shared::CardType type = order.allowed_type;
        unsigned int max_cost = order.max_cost;

        _mainGamePanel->drawGainFromBoard(*game_state, type, max_cost);
    }
} // namespace client
