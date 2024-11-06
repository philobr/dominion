
#include "enemy_info_panel.h"
#include <wx/wx.h>

namespace client
{

    EnemyInfoPanel::EnemyInfoPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        this->SetBackgroundColour(wxColour(255, 0, 0));
    }


} // namespace client
