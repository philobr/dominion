#pragma once

#include <shared/game/game_state/reduced_game_state.h>
#include <shared/player_result.h>
#include <wx/wx.h>

namespace client
{
    /**
     * @brief The panel that displays the victory screen
     */
    class VictoryScreenPanel : public wxPanel
    {
    public:
        VictoryScreenPanel(wxWindow *parent);

        /**
         * @brief Draw the victory screen
         */
        void drawVictoryScreen(const std::vector<shared::PlayerResult> &player_results);

        /**
         * @brief Draw the victory screen for testing purposes
         */
        void drawTestVictoryScreen();

    private:
        wxBoxSizer *victory_screen_sizer;

        void OnCloseButtonClicked(wxCommandEvent& /*event*/);
    };

} // namespace client
