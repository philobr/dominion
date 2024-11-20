#include "player_panel.h"
#include <wx/sizer.h>
#include <wx/wx.h>
#include "pile_panel.h"

namespace client
{

    PlayerPanel::PlayerPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        auto player = shared::PlayerBase("gigu");
        auto reduced = shared::ReducedPlayer::make(player, {"Copper", "Copper", "Copper", "Copper", "Estate"});
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
        wxPanel *hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        wxPanel *right = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        outersizer->Add(left, 1, wxEXPAND, 5);
        outersizer->Add(hand, 5, wxEXPAND, 5);
        outersizer->Add(right, 1, wxEXPAND, 5);

        this->SetSizer(outersizer);

        wxBoxSizer *outer_hand_sizer = new wxBoxSizer(wxVERTICAL);
        outer_hand_sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

        wxBoxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

        outer_hand_sizer->Add(sizer, 1, wxALIGN_CENTER_HORIZONTAL, 5);

        /*if (hand_size * card_width_borders > hand->GetSize().GetWidth()) {
            hand_card_size.SetWidth(hand->GetSize().GetWidth() / hand_size - 8);
            hand_card_size.SetHeight(hand_card_size.GetWidth() / 4 * 5);
        }*/

        for ( size_t i = 0; i < hand_size; i++ ) {
            PilePanel *card = new PilePanel(hand, shared::Pile::make(cards[i], 1));
            sizer->Add(card, 0, wxLEFT | wxRIGHT, 4);
        }

        /*std::cerr << "at least we're trying to draw a card\n";
        PilePanel * card = new PilePanel(this, shared::Pile::make(cards[0], 1));
        sizer->Add(card, 1, wxALL, 5);*/
        hand->SetSizer(sizer);
    }
} // namespace client
