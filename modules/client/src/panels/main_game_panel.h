#pragma once

#include "board_panel.h"
#include "enemy_info_panel.h"
#include "player_panel.h"

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
        void DrawGameState(std::unique_ptr<shared::ReducedGameState> &GameState);


    private:
        BoardPanel *Board;
        PlayerPanel *Player;
        EnemyInfoPanel *EnemyInfo;

        // define key constant layout values
        const wxSize VerticalBaseSize = wxSize(1024, 100);
        const wxSize PanelSize = wxSize(1024, 1024);
    };

} // namespace client
