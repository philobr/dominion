#include "main_game_panel.h"

#include <wx/wx.h>

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


    void MainGamePanel::DrawGameState(std::shared_ptr<shared::ReducedGameState> GameState) {
      Board->DrawBoard(GameState->board);
      Player->DrawPlayer(GameState->reduced_player);
      EnemyInfo->DrawEnemies(GameState->enemies);
    }

    


} // namespace client
