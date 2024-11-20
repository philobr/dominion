#include "player_panel.h"
#include <wx/wx.h>

namespace client
{

    PlayerPanel::PlayerPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        this->SetBackgroundColour(wxColour(0, 0, 255));
    }

    void PlayerPanel::DrawPlayer(const std::unique_ptr<shared::ReducedPlayer> &Player)
    {
        return;
    }
} // namespace client
