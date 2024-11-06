
#include "enemy_info_panel.h"
#include <wx/wx.h>

namespace client
{

    EnemyInfoPanel::EnemyInfoPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 100))
    {
        this->SetBackgroundColour(wxColour(255, 0, 0));
    }


} // namespace client
