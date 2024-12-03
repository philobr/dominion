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
        PhaseInfo = new PhaseInfoPanel(this, VerticalBaseSize);

        mainLayout->Add(EnemyInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 6, wxEXPAND, 10);
        mainLayout->Add(PhaseInfo, 1, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);

        this->SetSizerAndFit(mainLayout);
    }

    std::mutex syncMutex;
    std::condition_variable syncCv;
    bool taskCompleted = false;

    void MainGamePanel::drawGameState(const reduced::GameState &game_state)
    {
        bool is_active = (game_state.active_player == game_state.reduced_player->getId());

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
        PhaseInfo->drawInfoPanel(game_state);
        LOG(INFO) << "PhaseInfo drawn";
        Player->drawPlayer(game_state.reduced_player, is_active);

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
    }


} // namespace client
