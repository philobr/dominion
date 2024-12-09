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
        mainLayout->Add(Board, 8, wxEXPAND, 10);
        mainLayout->Add(PhaseInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);

        this->SetSizerAndFit(mainLayout);
    }

    void MainGamePanel::drawGameState(const reduced::GameState &game_state)
    {
        PhaseInfo->drawInfoPanel(game_state);
        Board->drawBoard(game_state.board, game_state.isPlayerActive(), game_state.reduced_player->getTreasure(),
                         game_state.game_phase);

        Player->drawPlayer(game_state.reduced_player, game_state.isPlayerActive(), game_state.game_phase);
        EnemyInfo->drawEnemies(game_state.reduced_enemies, game_state.active_player);
    }

    void MainGamePanel::drawGainFromBoard(const reduced::GameState &game_state, shared::CardType type,
                                          unsigned int max_cost)
    {
        PhaseInfo->drawInfoPanel(game_state);
        Board->drawBoard(game_state.board, max_cost, type);

        // set is_active to false to avoid any issues with players trying to play cards
        // during this phase
        Player->drawPlayer(game_state.reduced_player, false, game_state.game_phase);
        EnemyInfo->drawEnemies(game_state.reduced_enemies, game_state.active_player);
    }

} // namespace client
