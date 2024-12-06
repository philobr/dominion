#include <panels/player_panel.h>

#include <dominion.h>
#include <game_controller.h>
#include <shared/game/cards/card_factory.h>
#include <shared/utils/logger.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/single_card_panel.h>
#include <wx/wx.h>

namespace client
{

    PlayerPanel::PlayerPanel(wxWindow* parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        LOG(WARN) << "using hard coded player";
        auto player = shared::PlayerBase("gigu");
        auto reduced = reduced::Player::make(player, {"Village", "Copper", "Copper", "Copper", "Estate"});
        this->drawPlayer(reduced, true, shared::GamePhase::ACTION_PHASE);
    }

    void PlayerPanel::drawPlayer(const std::unique_ptr<reduced::Player> &player, bool is_active,
                                 shared::GamePhase phase)
    {
        // Remove old stuff
        this->DestroyChildren();

        LOG(INFO) << "Drawing player " << player->getId();
        // Create a sizer to hold the player stuff
        wxBoxSizer* outersizer = new wxBoxSizer(wxHORIZONTAL);

        // Set the minimum size of the panel
        size_t card_width_borders = hand_card_size.GetWidth() + 8;
        outersizer->SetMinSize(wxSize(7 * card_width_borders, 150));

        // Create the draw pile panel
        wxPanel* DrawPilePanel = createDrawPilePanel(player->getDrawPileSize());

        // Create the hand panel
        wxPanel *hand = createHandPanel(player, card_width_borders, is_active, phase);

        // Create the discard pile panel
        wxPanel* DiscardPilePanel = createDiscardPilePanel(player->getDiscardPileSize(), player->getTopDiscardCard());

        outersizer->Add(DrawPilePanel, 0, wxTOP, 5);
        outersizer->Add(hand, 1, wxTOP, 5);
        outersizer->Add(DiscardPilePanel, 0, wxTOP, 5);

        this->SetSizer(outersizer);
        this->Layout();
    }

    void PlayerPanel::makePlayable(SingleCardPanel* image, const std::string& card_id)
    {
        image->SetToolTip("Play card");

        image->SetCursor(wxCursor(wxCURSOR_HAND));

        image->setBorderColor(wxColour(255, 255, 255));

        // Bind left click on the panel to the buyCard function
        image->Bind(wxEVT_LEFT_UP,
            [card_id](wxMouseEvent& /*event*/) { wxGetApp().getController().playCard(card_id); });
    }

    wxPanel* PlayerPanel::createDrawPilePanel(const unsigned int draw_pile_size)
    {
        LOG(INFO) << "Creating draw pile panel";
        // Create the draw pile panel
        wxPanel* DrawPilePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        // Create the draw pile
        PilePanel* DrawPile = new PilePanel(DrawPilePanel, shared::Pile("Card_back", draw_pile_size),
            formatting_constants::DEFAULT_BOARD_PILE_SIZE);
        // Create the sizer for the draw pile
        wxBoxSizer* DrawPileSizer = new wxBoxSizer(wxVERTICAL);
        DrawPileSizer->SetMinSize(wxSize(1 * hand_card_size.GetWidth(), 150));
        // Add the draw pile to the sizer
        DrawPileSizer->Add(DrawPile, 0, wxALIGN_CENTER, 4);
        // Set the sizer for the draw pile panel
        DrawPilePanel->SetSizer(DrawPileSizer);

        return DrawPilePanel;
    }

    wxPanel *PlayerPanel::createHandPanel(const std::unique_ptr<reduced::Player> &player,
                                          const size_t card_width_borders, const bool is_active,
                                          shared::GamePhase phase)
    {
        // Get the hand cards
        const auto& cards = player->getHandCards();
        size_t hand_size = cards.size();

        // Create the hand panel
        wxPanel* hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        // Create the sizer for the hand
        wxGridSizer* sizer = new wxGridSizer(1, hand_size, 0, 0);
        sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

        // Set the sizer for the hand panel
        hand->SetSizer(sizer);

        // Set the size of the cards
        if (card_width_borders * hand_size > 724) {
            // scale bigger hands
            hand_card_size.SetWidth(724 / hand_size - 8);
            hand_card_size.SetHeight(hand_card_size.GetWidth() / 4 * 5);
        }

        // Add the cards to the hand
        for (size_t i = 0; i < hand_size; i++) {
            SingleCardPanel* card = new SingleCardPanel(hand, cards[i], hand_card_size, 5);

            bool is_action = shared::CardFactory::getCard(cards[i]).isAction();

            if ( is_action && (phase == shared::GamePhase::ACTION_PHASE) && is_active && player->getActions() > 0 ) {
                makePlayable(card, cards[i]);
            }
            sizer->Add(card, 0, wxALIGN_CENTER, 4);
        }

        // Set the sizer for the hand panel
        hand->Layout();
        return hand;
    }

    wxPanel* PlayerPanel::createDiscardPilePanel(const unsigned int discard_pile_size,
        const std::string& top_discard_card)
    {
        LOG(INFO) << "Creating discard pile panel";
        // Create the discard pile panel
        wxPanel* DiscardPilePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        // Declare DiscardPile outside the if/else blocks
        PilePanel* DiscardPile;

        // Create the discard pile
        if (discard_pile_size == 0) {
            DiscardPile = new PilePanel(DiscardPilePanel, shared::Pile("logo", 0),
                formatting_constants::DEFAULT_BOARD_PILE_SIZE);
        }
        else {
            DiscardPile = new PilePanel(DiscardPilePanel, shared::Pile(top_discard_card, discard_pile_size));
        }

        // Create the sizer for the discard pile
        wxBoxSizer* DiscardPileSizer = new wxBoxSizer(wxVERTICAL);
        DiscardPileSizer->SetMinSize(wxSize(1 * hand_card_size.GetWidth(), 150));

        // Add the discard pile to the sizer
        DiscardPileSizer->Add(DiscardPile, 0, wxALIGN_CENTER, 4);

        // Set the sizer for the discard pile panel
        DiscardPilePanel->SetSizer(DiscardPileSizer);

        return DiscardPilePanel;
    }

} // namespace client
