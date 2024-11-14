#pragma once

#include "board_panel.h"
#include "enemy_info_panel.h"
#include "player_panel.h"

#include <shared/game/game_state/reduced_game_state.h>

#include <memory>
#include <wx/wx.h>

namespace client
{

    class MainGamePanel : public wxPanel
    {

    public:
        explicit MainGamePanel(wxWindow *parent);

        void DrawGameState(std::shared_ptr<shared::ReducedGameState> GameState);


    private:
        // define key constant layout values
        //
        BoardPanel* Board;
        PlayerPanel* Player;
        EnemyInfoPanel* EnemyInfo;

        const wxSize VerticalBaseSize = wxSize(1024, 100);
        const wxSize PanelSize = wxSize(1024, 1024);
    };

} // namespace client
