#include <uiElements/pile_panel.h>
#include "uiElements/text_panel.h"


namespace client
{
    PilePanel::PilePanel(wxWindow *parent, shared::Pile pile, wxSize size, int padding) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), Pile(pile)
    {
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        std::string cardPath = "assets/" + pile.card_id + ".png";

        Image = new ImagePanel(this, cardPath, wxBITMAP_TYPE_PNG, wxDefaultPosition,
                               wxSize(size.GetWidth(), size.GetHeight() - 2 * padding));

        sizer->Add(Image, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

        TextPanel *PileSize = new TextPanel(this, wxID_ANY, std::to_string(pile.count), TextFormat::PLAIN);

        sizer->Add(PileSize, 0, wxALIGN_CENTER_HORIZONTAL);

        this->SetSizer(sizer);
    }


    shared::Pile PilePanel::getPile() const { return Pile; }

} // namespace client
