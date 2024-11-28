#include <panels/main_game_panel.h>
#include <shared/utils/logger.h>

#include <wx/wx.h>

namespace client
{

    MainGamePanel::MainGamePanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, PanelSize)
    {
        LOG(ERROR) << "Ctor MainGamePanel";
        wxBoxSizer *mainLayout = new wxBoxSizer(wxVERTICAL);
        LOG(WARN) << "MainLayout created";
        Board = new BoardPanel(this, VerticalBaseSize);
        LOG(WARN) << "BoardPanel created";
        EnemyInfo = new EnemyInfoPanel(this, VerticalBaseSize);
        LOG(WARN) << "EnemyInfoPanel created";
        Player = new PlayerPanel(this, VerticalBaseSize);
        LOG(WARN) << "PlayerPanel created";

        mainLayout->Add(EnemyInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 5, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);
        this->SetSizerAndFit(mainLayout);
        LOG(WARN) << "MainLayout set";
    }


    void MainGamePanel::drawGameState(const reduced::GameState &gameState)
    {
        LOG(INFO) << "Called MainGamePanel::drawGameState()";
        bool is_active = (gameState.active_player == gameState.reduced_player->getId());

        LOG(INFO) << "Set bool is_active for active player";
        Board->drawBoard(gameState.board, is_active, gameState.reduced_player->getTreasure());
        LOG(INFO) << "Board drawn";
        Player->drawPlayer(gameState.reduced_player, is_active);
        LOG(INFO) << "Player drawn";
        EnemyInfo->drawEnemies(gameState.reduced_enemies);
        LOG(INFO) << "Enemies drawn";
    }


} // namespace client
