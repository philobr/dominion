#include <uiElements/pile_panel.h>
#include "uiElements/text_panel.h"


namespace client
{
    PilePanel::PilePanel(wxWindow *parent, shared::Pile pile) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), Pile(pile)
    {
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        std::string CardPath = "assets/" + pile.card_id + ".png";

        Image = new ImagePanel(this, CardPath, wxBITMAP_TYPE_PNG, wxDefaultPosition, wxSize(100, 125));

        sizer->Add(Image, 0, wxALIGN_CENTER_HORIZONTAL);

        TextPanel *PileSize = new TextPanel(this, wxID_ANY, std::to_string(pile.count), TextFormat::Plain);

        sizer->Add(PileSize, 0, wxALIGN_CENTER_HORIZONTAL);

        this->SetSizer(sizer);
    }

    shared::Pile PilePanel::getPile() const { return Pile; }

} // namespace client
