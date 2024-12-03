#include <condition_variable>
#include <mutex>
#include <panels/main_game_panel.h>
#include <shared/utils/logger.h>

#include <wx/app.h>

namespace client
{

    MainGamePanel::MainGamePanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, PanelSize)
    {
        LOG(INFO) << "Ctor MainGamePanel";
        wxBoxSizer *mainLayout = new wxBoxSizer(wxVERTICAL);
        LOG(INFO) << "MainLayout created";
        Board = new BoardPanel(this, VerticalBaseSize);
        LOG(INFO) << "BoardPanel created";
        EnemyInfo = new EnemyInfoPanel(this, VerticalBaseSize);
        LOG(INFO) << "EnemyInfoPanel created";
        Player = new PlayerPanel(this, VerticalBaseSize);
        LOG(INFO) << "PlayerPanel created";
        PhaseInfo = new PhaseInfoPanel(this, VerticalBaseSize);
        LOG(INFO) << "PhaseInfoPanel created";

        mainLayout->Add(EnemyInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 5, wxEXPAND, 10);
        mainLayout->Add(PhaseInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);

        this->SetSizerAndFit(mainLayout);
        LOG(INFO) << "MainLayout set";
    }

    std::mutex syncMutex;
    std::condition_variable syncCv;
    bool taskCompleted = false;

    void MainGamePanel::drawGameState(const reduced::GameState &game_state)
    {
        LOG(INFO) << "Called MainGamePanel::drawGameState()";
        bool is_active = (game_state.active_player == game_state.reduced_player->getId());

        LOG(INFO) << "Set bool is_active for active player";

        // Synchronize Board->drawBoard
        {
            std::unique_lock<std::mutex> lock(syncMutex);
            taskCompleted = false; // Reset task status
            CallAfter(
                    [this, &game_state, is_active]()
                    {
                        Board->drawBoard(game_state.board, is_active, game_state.reduced_player->getTreasure());
                        {
                            std::lock_guard<std::mutex> lock(syncMutex);
                            taskCompleted = true; // Mark task as complete
                        }
                        syncCv.notify_one(); // Notify waiting thread
                    });
            syncCv.wait(lock, [] { return taskCompleted; }); // Wait until task is complete
        }
        LOG(INFO) << "Board drawn";
        PhaseInfo->drawInfoPanel(game_state.reduced_player, game_state.reduced_enemies, game_state.active_player);
        LOG(INFO) << "PhaseInfo drawn";
        Player->drawPlayer(game_state.reduced_player, is_active);
        LOG(INFO) << "Player drawn";

        // Synchronize EnemyInfo->drawEnemies
        {
            std::unique_lock<std::mutex> lock(syncMutex);
            taskCompleted = false; // Reset task status
            CallAfter(
                    [this, &game_state]()
                    {
                        EnemyInfo->drawEnemies(game_state.reduced_enemies);
                        {
                            std::lock_guard<std::mutex> lock(syncMutex);
                            taskCompleted = true; // Mark task as complete
                        }
                        syncCv.notify_one(); // Notify waiting thread
                    });
            syncCv.wait(lock, [] { return taskCompleted; }); // Wait until task is complete
        }
        LOG(INFO) << "Enemies drawn";
    }


} // namespace client
