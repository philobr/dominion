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


    void MainGamePanel::DrawGameState(const std::unique_ptr<reduced::GameState> &GameState)
    {

        bool is_active = (GameState->active_player == GameState->reduced_player->getId());


        Board->DrawBoard(GameState->board, is_active, GameState->reduced_player->getTreasure());
        Player->DrawPlayer(GameState->reduced_player, is_active);
        EnemyInfo->DrawEnemies(GameState->reduced_enemies);
    }


} // namespace client
