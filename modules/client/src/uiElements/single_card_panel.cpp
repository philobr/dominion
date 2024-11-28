#include <uiElements/single_card_panel.h>


namespace client
{
    SingleCardPanel::SingleCardPanel(wxWindow *parent, std::string card_name, wxSize size, int padding) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
    {
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        std::string CardPath = "assets/" + card_name + ".png";

        Image = new ImagePanel(this, CardPath, wxBITMAP_TYPE_PNG, wxDefaultPosition,
                               wxSize(size.GetWidth(), size.GetHeight() - 2 * padding));
        sizer->Add(Image, 0, wxALIGN_CENTER_HORIZONTAL);
        this->SetSizer(sizer);
    }
} // namespace client
