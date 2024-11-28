#pragma once


#include <wx/wx.h>
#include <shared/game/game_state/reduced_game_state.h>


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
         *
         * @param std::vector<std::string> player_names
         * @param std::vector<unsigned int> player_scores
         */
        void drawVictoryScreen(reduced::GameState &gameState);

    private:
        wxBoxSizer *victory_screen_sizer;
    };

} // namespace client
