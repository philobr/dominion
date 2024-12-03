#include <condition_variable>
#include <mutex>
#include <panels/main_game_panel.h>
#include <shared/utils/logger.h>

#include <wx/app.h>

namespace client
{

    MainGamePanel::MainGamePanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, PanelSize)
    {
        wxBoxSizer *mainLayout = new wxBoxSizer(wxVERTICAL);
        Board = new BoardPanel(this, VerticalBaseSize);
        EnemyInfo = new EnemyInfoPanel(this, VerticalBaseSize);
        Player = new PlayerPanel(this, VerticalBaseSize);

        mainLayout->Add(EnemyInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 5, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);
        this->SetSizerAndFit(mainLayout);
    }

    std::mutex syncMutex;
    std::condition_variable syncCv;
    bool taskCompleted = false;

    void MainGamePanel::drawGameState(const reduced::GameState &game_state)
    {
        bool is_active = (game_state.active_player == game_state.reduced_player->getId());

        Board->drawBoard(game_state.board, is_active, game_state.reduced_player->getTreasure());
        Player->drawPlayer(game_state.reduced_player, is_active);
        EnemyInfo->drawEnemies(game_state.reduced_enemies);
    }

} // namespace client
