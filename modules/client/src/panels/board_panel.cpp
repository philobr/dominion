#include "board_panel.h"
#include <wx/wx.h>

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 100))
    {
        this->SetBackgroundColour(wxColour(0, 255, 0));
    }


} // namespace client
