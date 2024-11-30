#include <panels/enemy_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/enemy_panel.h>
#include <wx/wx.h>

namespace client
{
    void EnemyInfoPanel::drawEnemies(const std::vector<std::unique_ptr<reduced::Enemy>> &enemies)
    {
        this->DestroyChildren();

        auto *sizer = new wxGridSizer(1, enemies.size(), 10, 10);

        for ( const auto &Enemy : enemies ) {
            EnemyPanel *enemy_panel = new EnemyPanel(this, *Enemy);
            sizer->Add(enemy_panel, 0, wxALIGN_CENTER_HORIZONTAL);
        }

        this->SetSizer(sizer);
    }
} // namespace client
