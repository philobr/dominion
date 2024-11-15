#include "pile_panel.h"


namespace client
{
    PilePanel::PilePanel(wxWindow *parent, shared::Pile pile) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
    {
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        std::string CardPath = "assets/" + pile.card_id + ".png";

        ImagePanel *image_panel =
                new ImagePanel(this, CardPath, wxBITMAP_TYPE_PNG, wxDefaultPosition, wxSize(100, 100));

        sizer->Add(image_panel, 0, wxALIGN_CENTER_HORIZONTAL);

        wxStaticText *PileSize = new wxStaticText(this, wxID_ANY, std::to_string(pile.count), wxDefaultPosition,
                                                  wxDefaultSize, wxALIGN_CENTER);

        PileSize->SetForegroundColour(wxColor(0, 0, 0));

        sizer->Add(PileSize, 0, wxALIGN_CENTER_HORIZONTAL);

        this->SetSizer(sizer);
    }
} // namespace client
