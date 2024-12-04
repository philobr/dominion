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

        Bind(wxEVT_PAINT, &PilePanel::OnPaint, this);
    }

    void PilePanel::greyOut()
    {
        m_isGreyedOut = true;
        Refresh();  // Request a repaint
        Update();   // Force immediate repaint
    }

    void PilePanel::OnPaint(wxPaintEvent& event)
    {
        wxPaintDC dc(this);
        event.Skip();  // Allow normal painting first

        if (m_isGreyedOut) {
            wxColour grayOverlay(128, 128, 128, 128);
            dc.SetBrush(wxBrush(grayOverlay));
            dc.SetPen(*wxTRANSPARENT_PEN);
            dc.DrawRectangle(GetClientRect());
        }
    }
    shared::Pile PilePanel::getPile() const { return Pile; }

} // namespace client
