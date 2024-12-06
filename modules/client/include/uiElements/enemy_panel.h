#pragma once

#include <shared/game/game_state/reduced_game_state.h>

#include <wx/wx.h>


namespace client
{
    /**
     * @brief A utility panel that displays a single enemy
     */
    class EnemyPanel : public wxPanel
    {
    public:
        EnemyPanel(wxWindow *parent, reduced::Enemy &enemy, const bool is_active);

    private:
        void onPaint(wxPaintEvent &event);

        bool _is_active;
    }; // namespace client
} // namespace client
