#include "player_panel.h"
#include <wx/sizer.h>
#include <wx/wx.h>
#include "pile_panel.h"

namespace client
{

    PlayerPanel::PlayerPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        auto player = shared::PlayerBase("gigu");
        auto reduced = shared::ReducedPlayer::make(
                player, {"Copper", "Copper", "Copper", "Copper", "Estate", "Estate", "Estate", "Estate", "Estate"});
        this->DrawPlayer(reduced);
    }

    void PlayerPanel::DrawPlayer(const std::unique_ptr<shared::ReducedPlayer> &Player)
    {

        const auto &cards = Player->getHandCards();
        size_t hand_size = cards.size();

        wxBoxSizer *outersizer = new wxBoxSizer(wxHORIZONTAL);

        size_t card_width_borders = hand_card_size.GetWidth() + 8;
        outersizer->SetMinSize(wxSize(7 * card_width_borders, 150));

        wxPanel *left = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        left->SetBackgroundColour(wxColour(255, 0, 0));
        wxBoxSizer *left_sizer = new wxBoxSizer(wxVERTICAL);
        left_sizer->SetMinSize(wxSize(1 * card_width_borders, 150));
        left->SetSizer(left_sizer);


        wxPanel *hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        wxPanel *right = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        wxBoxSizer *right_sizer = new wxBoxSizer(wxVERTICAL);
        right_sizer->SetMinSize(wxSize(1 * card_width_borders, 150));
        right->SetSizer(right_sizer);

        right->SetBackgroundColour(wxColour(0, 0, 255));

        outersizer->Add(left, 1, wxTOP, 5);
        outersizer->Add(hand, 5, wxTOP, 5);
        outersizer->Add(right, 1, wxTOP, 5);

        this->SetSizer(outersizer);

        wxGridSizer *sizer = new wxGridSizer(1, hand_size, 0, 0);
        sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

        hand->SetSizer(sizer);

        if ( hand_size > 5 ) {
            // scale bigger hands
            hand_card_size.SetWidth(hand_card_size.GetWidth() * 5 / hand_size - 8);
            hand_card_size.SetHeight(hand_card_size.GetWidth() / 4 * 5);
        }

        for ( size_t i = 0; i < hand_size; i++ ) {
            ImagePanel *card = new ImagePanel(hand, "assets/" + cards[i] + ".png", wxBITMAP_TYPE_PNG, wxDefaultPosition,
                                              hand_card_size, 0);
            sizer->Add(card, 0, wxLEFT | wxRIGHT, 4);
        }

        sizer->Layout();

        /*std::cerr << "at least we're trying to draw a card\n";
        PilePanel * card = new PilePanel(this, shared::Pile::make(cards[0], 1));
        sizer->Add(card, 1, wxALL, 5);*/
    }
} // namespace client
