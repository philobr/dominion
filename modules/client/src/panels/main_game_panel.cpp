#include <panels/main_game_panel.h>

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


    void MainGamePanel::drawGameState(const reduced::GameState &gameState)
    {

        bool is_active = (gameState.active_player == gameState.reduced_player->getId());


        Board->drawBoard(gameState.board, is_active, gameState.reduced_player->getTreasure());
        Player->drawPlayer(gameState.reduced_player, is_active);
        EnemyInfo->drawEnemies(gameState.reduced_enemies);
    }


} // namespace client
