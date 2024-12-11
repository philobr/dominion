#include <uiElements/single_card_panel.h>

// NOLINTBEGIN(bugprone-suspicious-enum-usage)
namespace client
{
    SingleCardPanel::SingleCardPanel(wxWindow *parent, const std::string &card_name, wxSize size, int padding) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), CardName(card_name)
    {
        wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);

        std::string asset_name = card_name + ".png";

        Image = new ImagePanel(this, asset_name, wxBITMAP_TYPE_PNG, wxDefaultPosition,
                               wxSize(size.GetWidth() - 2 * padding, size.GetHeight() - 2 * padding));
        sizer->Add(Image, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, padding);
        this->SetSizer(sizer);
    }

    void SingleCardPanel::setBorderColor(const wxColour &color)
    {
        this->SetBackgroundColour(color); // Set background color
        this->Refresh(); // Trigger a repaint
    }
} // namespace client
// NOLINTEND(bugprone-suspicious-enum-usage)
