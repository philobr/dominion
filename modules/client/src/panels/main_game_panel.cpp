#include "main_game_panel.h"

#include "board_panel.h"
#include "enemy_info_panel.h"
#include "player_panel.h"

#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>

namespace client
{

    MainGamePanel::MainGamePanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024))
    {
        wxBoxSizer *mainLayout = new wxBoxSizer(wxVERTICAL);

        BoardPanel *Board = new BoardPanel(this);
        wxPanel *Enemies = new EnemyInfoPanel(this);
        wxPanel *Player = new PlayerPanel(this);

        mainLayout->Add(Enemies, 1, wxEXPAND, 10);
        mainLayout->Add(Board, 4, wxEXPAND, 10);
        mainLayout->Add(Player, 2, wxEXPAND, 10);
        this->SetSizerAndFit(mainLayout);
    }


} // namespace client
