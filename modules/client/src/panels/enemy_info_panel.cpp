#include <panels/enemy_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/enemy_panel.h>
#include <wx/wx.h>

namespace client
{
    EnemyInfoPanel::EnemyInfoPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        // create a dummy enemy for testing
        auto player = shared::PlayerBase("gigu");
        auto enemy = reduced::Enemy::make(player, 5);
        std::vector<std::unique_ptr<reduced::Enemy>> enemies;
        auto enemy2 = reduced::Enemy::make(player, 5);
        enemies.push_back(std::move(enemy2));
        enemies.push_back(std::move(enemy));
        drawEnemies(enemies);
    }

    void EnemyInfoPanel::drawEnemies(const std::vector<std::unique_ptr<reduced::Enemy>> &enemies)
    {
        this->DestroyChildren();

        auto *sizer = new wxGridSizer(1, enemies.size(), 10, 10);

        for ( const auto &Enemy : enemies ) {
            EnemyPanel *enemy_panel = new EnemyPanel(this, *Enemy);
            sizer->Add(enemy_panel, 0, wxALIGN_CENTER_HORIZONTAL);
        }

        this->SetSizer(sizer);
        this->Layout();
    }
} // namespace client
