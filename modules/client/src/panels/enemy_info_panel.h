#pragma once

#include <shared/game/game_state/reduced_game_state.h>

#include <vector>
#include <wx/wx.h>

namespace client
{
    class EnemyInfoPanel : public wxPanel
    {
    public:
        EnemyInfoPanel(wxWindow *parent, wxSize size);

        void DrawEnemies(std::vector<shared::ReducedEnemy> &Enemies){};

    private:
    };

} // namespace client
