#include <panels/enemy_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/enemy_panel.h>
#include <wx/wx.h>

namespace client
{

    EnemyInfoPanel::EnemyInfoPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        // TODO: remove
        // just for temporary testing
        LOG(WARN) << "Ctor EnemyInfoPanel";
        std::vector<std::unique_ptr<reduced::Enemy>> enemies;
        for ( int i = 0; i < 3; i++ ) {
            auto enemy = reduced::Enemy::make(shared::PlayerBase("enemy" + std::to_string(i)), i + 4);
            enemies.push_back(std::move(enemy));
            LOG(INFO) << "Enemy " << i << " created";
        }

        drawEnemies(enemies);
    }

    void EnemyInfoPanel::drawEnemies(const std::vector<std::unique_ptr<reduced::Enemy>> &Enemies)
    {
        this->DestroyChildren();

        auto *sizer = new wxGridSizer(1, Enemies.size(), 10, 10);

        for ( const auto &Enemy : Enemies ) {
            EnemyPanel *enemy_panel = new EnemyPanel(this, *Enemy);
            sizer->Add(enemy_panel, 0, wxALIGN_CENTER_HORIZONTAL);
            LOG(DEBUG) << "EnemyPanel added";
        }

        this->SetSizer(sizer);
    }
} // namespace client
