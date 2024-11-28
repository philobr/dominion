#include <panels/enemy_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/enemy_panel.h>
#include <wx/wx.h>

namespace client
{
    void EnemyInfoPanel::drawEnemies(const std::vector<std::unique_ptr<reduced::Enemy>> &Enemies)
    {
        this->DestroyChildren();

        auto *sizer = new wxGridSizer(1, Enemies.size(), 10, 10);

        for ( const auto &Enemy : Enemies ) {
            EnemyPanel *enemy_panel = new EnemyPanel(this, *Enemy);
            sizer->Add(enemy_panel, 0, wxALIGN_CENTER_HORIZONTAL);
            LOG(INFO) << "EnemyPanel for " << Enemy->getId() << " added";
        }

        this->SetSizer(sizer);
    }
} // namespace client
