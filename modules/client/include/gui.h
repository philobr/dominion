
#pragma once

#include <panels/connection_panel.h>
#include <panels/lobby_panel.h>
#include <panels/main_game_panel.h>
#include <panels/victory_screen_panel.h>
#include <windows/game_window.h>

namespace client
{

    /**
     * @brief Structure representing the data entered by the user in the connection screen
     */
    struct ConnectionForm
    {
        std::string host;
        uint16_t port;
        std::string player_name;
        std::string lobby_name;
    };

    /**
     * @brief Class managing the graphical user interface
     *
     * This class is responsible for displaying the different screens of the game.
     * It is a wrapper around wxWidgets, and uses the GameWindow class to display the game.
     */
    class Gui
    {
    public:
        Gui(GameWindow *game_window);
        ~Gui() = default;

        bool getConnectionForm(ConnectionForm &connection_form);

        /**
         * @brief display an error message
         */
        void showError(const std::string &title, const std::string &message);

        /**
         * @brief display a status message in the status bar
         */
        void showStatus(const std::string &message);

        void showLobbyScreen(const std::vector<reduced::Player::id_t> &players, bool is_game_master);

        // TODO(#169): This should take the game state as an argument
        void showGameScreen();

        // TODO(#169): This shoul be removed and be part of the showGameScreen method
        void drawGameState(const reduced::GameState &game_state);

        void showVictoryScreen();


    private:
        GameWindow *_gameWindow;
        ConnectionPanel *_connectionPanel;
        MainGamePanel *_mainGamePanel;
        LobbyPanel *_lobbyPanel;
        VictoryScreenPanel *_victoryScreenPanel;
    };
} // namespace client
