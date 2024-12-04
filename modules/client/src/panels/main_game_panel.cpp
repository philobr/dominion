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

        // set size of the panel
        // This seems to fix an issue on the side of wxWidgets where the
        // panel size is left uninitialized
        this->SetSize(PanelSize);
        mainLayout->SetMinSize(PanelSize);
        Board = new BoardPanel(this, VerticalBaseSize);
        EnemyInfo = new EnemyInfoPanel(this, VerticalBaseSize);
        Player = new PlayerPanel(this, VerticalBaseSize);
        PhaseInfo = new PhaseInfoPanel(this, VerticalBaseSize);

        mainLayout->Add(EnemyInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 6, wxEXPAND, 10);
        mainLayout->Add(PhaseInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);

        this->SetSizerAndFit(mainLayout);
    }

    void MainGamePanel::drawGameState(const reduced::GameState &game_state)
    {
        bool is_active = (game_state.active_player == game_state.reduced_player->getId());
        PhaseInfo->drawInfoPanel(game_state);
        Board->drawBoard(game_state.board, is_active, game_state.reduced_player->getTreasure(), game_state.game_phase);

        Player->drawPlayer(game_state.reduced_player, is_active, game_state.game_phase);
        EnemyInfo->drawEnemies(game_state.reduced_enemies);
    }

} // namespace client
