#include "board_panel.h"
#include <wx/wx.h>
#include "pile_panel.h"

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        wxGridSizer *sizer = new wxGridSizer(3, 10, 0, 0);

        PilePanel *EstatePanel = new PilePanel(this, shared::Pile("Estate", 10));
        PilePanel *DuchyPanel = new PilePanel(this, shared::Pile("Duchy", 10));

        sizer->Add(EstatePanel, 0, wxALIGN_CENTER_HORIZONTAL);
        sizer->Add(DuchyPanel, 0, wxALIGN_CENTER_HORIZONTAL);
        this->SetSizer(sizer);
    }


} // namespace client
