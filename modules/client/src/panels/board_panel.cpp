#include "board_panel.h"
#include <wx/gbsizer.h>
#include <wx/wx.h>
#include "pile_panel.h"

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        wxGridBagSizer *sizer = new wxGridBagSizer(10, 10);

        wxGBPosition EstatePosition = wxGBPosition(0, 0);
        wxGBPosition DuchyPosition = wxGBPosition(1, 0);
        wxGBPosition ProvincePosition = wxGBPosition(2, 0);

        wxGBPosition CopperPosition = wxGBPosition(0, 1);
        wxGBPosition SilverPosition = wxGBPosition(1, 1);
        wxGBPosition GoldPosition = wxGBPosition(2, 1);

        wxGBSpan span = wxGBSpan(1, 1);


        PilePanel *EstatePanel = new PilePanel(this, shared::Pile("Estate", 10));
        PilePanel *DuchyPanel = new PilePanel(this, shared::Pile("Duchy", 10));
        PilePanel *ProvincePanel = new PilePanel(this, shared::Pile("Province", 10));

        PilePanel *CopperPanel = new PilePanel(this, shared::Pile("Copper", 10));
        PilePanel *SilverPanel = new PilePanel(this, shared::Pile("Silver", 10));
        PilePanel *GoldPanel = new PilePanel(this, shared::Pile("Gold", 10));

        sizer->Add(EstatePanel, EstatePosition, span, wxALIGN_CENTER_HORIZONTAL);
        sizer->Add(DuchyPanel, DuchyPosition, span, wxALIGN_CENTER_HORIZONTAL);
        sizer->Add(ProvincePanel, ProvincePosition, span, wxALIGN_CENTER_HORIZONTAL);
        sizer->Add(CopperPanel, CopperPosition, span, wxALIGN_CENTER_HORIZONTAL);
        sizer->Add(SilverPanel, SilverPosition, span, wxALIGN_CENTER_HORIZONTAL);
        sizer->Add(GoldPanel, GoldPosition, span, wxALIGN_CENTER_HORIZONTAL);
        this->SetSizer(sizer);
    }


} // namespace client
