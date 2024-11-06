#include "player_panel.h"
#include <wx/wx.h>

namespace client
{

    PlayerPanel::PlayerPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 100))
    {
        this->SetBackgroundColour(wxColour(0, 0, 255));
    }


} // namespace client
