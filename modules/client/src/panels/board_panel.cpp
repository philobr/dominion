#include "board_panel.h"
#include <wx/wx.h>

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        this->SetBackgroundColour(wxColour(0, 255, 0));
    }


} // namespace client
