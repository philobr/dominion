#include <uiElements/pile_panel.h>
#include <wx/filename.h>
#include "uiElements/text_panel.h"


namespace client
{
    // NOLINTBEGIN(bugprone-suspicious-enum-usage)
    PilePanel::PilePanel(wxWindow *parent, shared::Pile pile, wxSize size, int padding) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), Pile(pile)
    {
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        std::string asset_name = pile.card_id;

        Image = new ImagePanel(this, asset_name, wxBITMAP_TYPE_PNG, wxDefaultPosition,
                               wxSize(size.GetWidth(), size.GetHeight() - 2 * padding));

#ifdef __APPLE__
        sizer->Add(Image, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 5);
#else
        sizer->Add(Image, 0, wxALIGN_CENTER_HORIZONTAL | wxBOTTOM, 0);
#endif

        TextPanel *PileSize = new TextPanel(this, wxID_ANY, std::to_string(pile.count), TextFormat::BOLD_SMALL);

        sizer->Add(PileSize, 0, wxALIGN_CENTER_HORIZONTAL);

        this->SetSizer(sizer);
    }

    void PilePanel::makeGrey() { Image->makeGrey(wxString(Pile.card_id)); }


    shared::Pile PilePanel::getPile() const { return Pile; }
    // NOLINTEND(bugprone-suspicious-enum-usage)
} // namespace client
