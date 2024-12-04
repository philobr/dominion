#pragma once

#include <panels/board_panel.h>
#include <panels/enemy_info_panel.h>
#include <panels/phase_info_panel.h>
#include <panels/player_panel.h>

#include <shared/game/game_state/reduced_game_state.h>

#include <wx/wx.h>

namespace client
{
    /**
     * @brief The main panel that displays the game state
     *
     */
    class MainGamePanel : public wxPanel
    {

    public:
        explicit MainGamePanel(wxWindow *parent);

        /**
         * @brief Draw the game state
         *
         * @param GameState
         */
        void drawGameState(const reduced::GameState &game_state);


    private:
        BoardPanel *Board;
        PlayerPanel *Player;
        EnemyInfoPanel *EnemyInfo;
        PhaseInfoPanel *PhaseInfo;
        // define key constant layout values
        const wxSize VerticalBaseSize = wxSize(1024, 100);
        const wxSize PanelSize = wxSize(1024, 1024);
    };

} // namespace client
