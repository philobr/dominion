#include "main_game_panel.h"

#include "board_panel.h"
#include "enemy_info_panel.h"
#include "player_panel.h"

#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>

namespace client
{

    MainGamePanel::MainGamePanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, PanelSize)
    {
        wxBoxSizer *mainLayout = new wxBoxSizer(wxVERTICAL);

        BoardPanel *Board = new BoardPanel(this, VerticalBaseSize);
        wxPanel *Enemies = new EnemyInfoPanel(this, VerticalBaseSize);
        wxPanel *Player = new PlayerPanel(this, VerticalBaseSize);

        mainLayout->Add(Enemies, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 5, wxEXPAND, 10);
        mainLayout->Add(Player, 3, wxEXPAND, 10);
        this->SetSizerAndFit(mainLayout);
    }


} // namespace client
