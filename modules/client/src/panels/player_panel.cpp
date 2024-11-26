#include <panels/player_panel.h>

#include <game_controller.h>
#include <shared/game/cards/card_factory.h>
#include <shared/utils/logger.h>
#include <uiElements/image_panel.h>
#include <wx/event.h>
#include <wx/sizer.h>
#include <wx/wx.h>

namespace client
{

    PlayerPanel::PlayerPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        auto player = shared::PlayerBase("gigu");
        auto reduced = reduced::Player::make(player, {"Village", "Copper", "Copper", "Copper", "Estate"});
        this->DrawPlayer(reduced, true);
    }

    void PlayerPanel::DrawPlayer(const std::unique_ptr<reduced::Player> &Player, bool is_active)
    {

        const auto &cards = Player->getHandCards();
        size_t hand_size = cards.size();

        wxBoxSizer *outersizer = new wxBoxSizer(wxHORIZONTAL);

        size_t card_width_borders = hand_card_size.GetWidth() + 8;
        outersizer->SetMinSize(wxSize(7 * card_width_borders, 150));

        wxPanel *DrawPilePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        DrawPilePanel->SetBackgroundColour(wxColour(255, 0, 0));
        wxBoxSizer *DrawPileSizer = new wxBoxSizer(wxVERTICAL);
        DrawPileSizer->SetMinSize(wxSize(1 * card_width_borders, 150));
        DrawPilePanel->SetSizer(DrawPileSizer);


        wxPanel *hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        wxPanel *DiscardPilePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        wxBoxSizer *DiscardPileSizer = new wxBoxSizer(wxVERTICAL);
        DiscardPileSizer->SetMinSize(wxSize(1 * card_width_borders, 150));
        DiscardPilePanel->SetSizer(DiscardPileSizer);

        DiscardPilePanel->SetBackgroundColour(wxColour(0, 0, 255));

        outersizer->Add(DrawPilePanel, 0, wxTOP, 5);
        outersizer->Add(hand, 1, wxTOP, 5);
        outersizer->Add(DiscardPilePanel, 0, wxTOP, 5);

        this->SetSizer(outersizer);

        wxGridSizer *sizer = new wxGridSizer(1, hand_size, 0, 0);
        sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

        hand->SetSizer(sizer);
        LOG(DEBUG) << "Hand panel width " << hand->GetSize().GetWidth() << "\n";
        LOG(DEBUG) << "card width " << card_width_borders << "\n";

        if ( card_width_borders * hand_size > 724 ) {
            // scale bigger hands
            hand_card_size.SetWidth(724 / hand_size - 8);
            hand_card_size.SetHeight(hand_card_size.GetWidth() / 4 * 5);
        }

        for ( size_t i = 0; i < hand_size; i++ ) {
            ImagePanel *card = new ImagePanel(hand, "assets/" + cards[i] + ".png", wxBITMAP_TYPE_PNG, wxDefaultPosition,
                                              hand_card_size, 0);

            bool is_action = shared::CardFactory::getCard(cards[i]).isAction();

            if ( is_action && is_active && Player->getActions() > 0 ) {
                makePlayable(card, cards[i]);
            }

            sizer->Add(card, 0, wxALIGN_CENTER, 4);
        }

        sizer->Layout();
    }

    void PlayerPanel::makePlayable(ImagePanel *Image, std::string card_id)
    {
        Image->SetToolTip("Play card");

        Image->SetCursor(wxCursor(wxCURSOR_HAND));

        // Bind left click on the panel to the buyCard function
        Image->Bind(wxEVT_LEFT_UP, [card_id](wxMouseEvent & /*event*/) { GameController::playCard(card_id); });
    }
} // namespace client
