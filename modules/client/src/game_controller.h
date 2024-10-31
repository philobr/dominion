#ifndef LAMAUI_GAMECONTROLLER_H
#define LAMAUI_GAMECONTROLLER_H

#include "windows/game_window.h"
#include "panels/connection_panel.h"
#include "panels/main_game_panel.h"


class GameController {

public:
    static void init(GameWindow* gameWindow);

    static void connectToServer();
    static void startGame();
    static void drawCard();
    static void fold();

    static wxEvtHandler* getMainThreadEventHandler();
    static void showError(const std::string& title, const std::string& message);
    static void showStatus(const std::string& message);
    static void showGameOverMessage();

private:
    static GameWindow* _gameWindow;
    static ConnectionPanel* _connectionPanel;
    static MainGamePanel* _mainGamePanel;

};


#endif //LAMAUI_GAMECONTROLLER_H
