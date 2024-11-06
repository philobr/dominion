#pragma once

#include "panels/connection_panel.h"
#include "panels/main_game_panel.h"
#include "windows/game_window.h"

namespace client
{

    class GameController
    {

    public:
        static void init(GameWindow *gameWindow);

        static void connectToServer();

        static void showError(const std::string &title, const std::string &message);
        static void showStatus(const std::string &message);

    private:
        static GameWindow *_gameWindow;
        static ConnectionPanel *_connectionPanel;
        static MainGamePanel *_mainGamePanel;
    };

} // namespace client
