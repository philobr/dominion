#include <panels/player_panel.h>

#include <dominion.h>
#include <game_controller.h>
#include <shared/game/cards/card_factory.h>
#include <shared/utils/logger.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/popup.h>
#include <uiElements/single_card_panel.h>
#include <wx/statline.h>
#include <wx/wx.h>
#include "shared/action_order.h"

namespace client
{
    // NOLINTBEGIN(bugprone-suspicious-enum-usage)
    PlayerPanel::PlayerPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        if ( wxGetApp().isDebugMode() ) {
            LOG(WARN) << "Using hard coded player";
            auto player = shared::PlayerBase("gigu");
            auto reduced = reduced::Player::make(player, {"Village", "Copper", "Copper", "Copper", "Estate"});
            this->drawSelectFromHandPlayer(reduced, 2, 3, shared::ChooseFromOrder::AllowedChoice::HAND_CARDS);
        }
    }

    void PlayerPanel::drawPlayer(const std::unique_ptr<reduced::Player> &player, bool is_active,
                                 shared::GamePhase phase, bool confirm_button,
                                 shared::ChooseFromOrder::AllowedChoice allowed_choices)
    {
        // Remove old stuff
        this->DestroyChildren();
        handPanels.clear();

        LOG(INFO) << "Drawing player " << player->getId();
        // Create a sizer to hold the player stuff
        wxBoxSizer *outersizer = new wxBoxSizer(wxHORIZONTAL);

        // Set the minimum size of the panel
        size_t card_width_borders = hand_card_size.GetWidth() + 8;
        outersizer->SetMinSize(wxSize(7 * card_width_borders, 150));

        // Create the draw pile panel
        wxPanel *DrawPilePanel = createDrawPilePanel(player->getDrawPileSize());

        // Create the hand panel
        wxPanel *hand = createHandPanel(player, card_width_borders, is_active, phase);

        // Create the discard pile panel
        wxPanel *DiscardPilePanel = createDiscardPilePanel(player->getDiscardPileSize(), player->getTopDiscardCard(),
                                                           confirm_button, allowed_choices);


        // Add a vertical line between the draw pile and the hand
        wxStaticLine *line1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(5, 120), wxLI_VERTICAL);
        wxStaticLine *line2 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxSize(5, 120), wxLI_VERTICAL);

        outersizer->Add(DrawPilePanel, 0, wxLEFT | wxTOP, 15);
        outersizer->Add(line1, 0, wxEXPAND | wxALL, 5);
        outersizer->Add(hand, 1, wxTOP, 5);
        outersizer->Add(line2, 0, wxEXPAND | wxALL, 5);
        outersizer->Add(DiscardPilePanel, 0, wxRIGHT | wxTOP, 15);

        this->SetSizer(outersizer);
        this->Layout();
    }

    void PlayerPanel::drawSelectFromHandPlayer(const std::unique_ptr<reduced::Player> &player, unsigned min_count,
                                               unsigned max_count,
                                               shared::ChooseFromOrder::AllowedChoice allowed_choices)
    {
        minCount = min_count;
        maxCount = max_count;
        drawPlayer(player, false, shared::GamePhase::ACTION_PHASE, true, allowed_choices);

        for ( auto &card : handPanels ) {
            // currently making every card selectable
            // we can change this as soon as we have the types of cards
            makeSelectable(card);
        }
    }

    void PlayerPanel::makePlayable(SingleCardPanel *image, const std::string &card_id)
    {
        image->SetToolTip("Play card");

        image->SetCursor(wxCursor(wxCURSOR_HAND));

        image->setBorderColor(formatting_constants::PLAYABLE_HAND_CARD_BORDER);

        // Bind left click on the panel to the buyCard function
        image->makeClickable(wxEVT_LEFT_UP,
                             [card_id](wxMouseEvent & /*event*/) { wxGetApp().getController().playCard(card_id); });
    }

    wxPanel *PlayerPanel::createDrawPilePanel(const unsigned int draw_pile_size)
    {
        LOG(INFO) << "Creating draw pile panel";
        // Create the draw pile panel
        wxPanel *DrawPilePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        // Create the draw pile
        PilePanel *DrawPile;
        if ( draw_pile_size == 0 ) {
            DrawPile = new PilePanel(DrawPilePanel, shared::Pile("empty_panel", draw_pile_size),
                                     formatting_constants::DEFAULT_BOARD_PILE_SIZE);
        } else {
            DrawPile = new PilePanel(DrawPilePanel, shared::Pile("Card_back", draw_pile_size),
                                     formatting_constants::DEFAULT_BOARD_PILE_SIZE);
        }
        // Create the sizer for the draw pile
        wxBoxSizer *DrawPileSizer = new wxBoxSizer(wxVERTICAL);
        DrawPileSizer->SetMinSize(wxSize(1 * hand_card_size.GetWidth(), 150));
        // Add the draw pile to the sizer
        DrawPileSizer->Add(DrawPile, 0, wxALIGN_CENTER, 4);

        SetBackgroundColour(formatting_constants::DEFAULT_PANEL_BACKGROUND);

        // Set the sizer for the draw pile panel
        DrawPilePanel->SetSizer(DrawPileSizer);

        return DrawPilePanel;
    }

    wxPanel *PlayerPanel::createHandPanel(const std::unique_ptr<reduced::Player> &player,
                                          const size_t card_width_borders, const bool is_active,
                                          shared::GamePhase phase)
    {
        // Get the hand cards
        const auto &cards = player->getHandCards();
        size_t hand_size = cards.size();

        wxSize hand_card_size = formatting_constants::DEFAULT_HAND_CARD_SIZE;

        // Create the hand panel
        wxPanel *hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        // Create the sizer for the hand
        wxGridSizer *sizer = new wxGridSizer(1, hand_size, 0, 0);
        sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

        // Set the sizer for the hand panel
        hand->SetSizer(sizer);

        // Set the size of the cards
        if ( card_width_borders * hand_size > 724 ) {
            // scale bigger hands
            hand_card_size.SetWidth(724 / hand_size - 8);
            hand_card_size.SetHeight(hand_card_size.GetWidth() / 4 * 5);
        }

        // Add the cards to the hand
        for ( size_t i = 0; i < hand_size; i++ ) {
            SingleCardPanel *card = new SingleCardPanel(hand, cards[i], hand_card_size, 5);
            handPanels.push_back(card);
            bool is_action = shared::CardFactory::getCard(cards[i]).isAction();

            // bind right click to show card preview
            card->makeClickable(wxEVT_RIGHT_UP,
                                [card](wxMouseEvent & /*event*/) { showCardPopup(card, card->getCardName()); });

            if ( is_action && (phase == shared::GamePhase::ACTION_PHASE) && is_active && player->getActions() > 0 ) {
                makePlayable(card, cards[i]);
            }
            sizer->Add(card, 0, wxALIGN_CENTER, 4);
        }

        // Set the sizer for the hand panel
        hand->Layout();
        return hand;
    }

    wxPanel *PlayerPanel::createDiscardPilePanel(const unsigned int discard_pile_size,
                                                 const std::string &top_discard_card, bool confirm_button,
                                                 shared::ChooseFromHandOrder::AllowedChoice allowed_choices)
    {
        LOG(INFO) << "Creating discard pile panel";
        // Create the discard pile panel
        wxPanel *DiscardPilePanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        // Declare DiscardPile outside the if/else blocks
        PilePanel *DiscardPile;

        // Create the discard pile
        if ( discard_pile_size == 0 ) {
            DiscardPile = new PilePanel(DiscardPilePanel, shared::Pile("empty_panel", discard_pile_size),
                                        formatting_constants::DEFAULT_BOARD_PILE_SIZE);
        } else {
            DiscardPile = new PilePanel(DiscardPilePanel, shared::Pile(top_discard_card, discard_pile_size),
                                        formatting_constants::DEFAULT_BOARD_PILE_SIZE);
        }

        // Create the sizer for the discard pile
        wxBoxSizer *DiscardPileSizer = new wxBoxSizer(wxVERTICAL);
        DiscardPileSizer->SetMinSize(wxSize(1 * hand_card_size.GetWidth(), 150));

        if ( confirm_button ) {
            confirmButton = new wxButton(DiscardPilePanel, wxID_ANY, "Confirm", wxDefaultPosition, wxDefaultSize);
            confirmButton->Bind(wxEVT_BUTTON,
                                [this, allowed_choices](wxCommandEvent & /*event*/)
                                {
                                    // get the relevant data for the request from the SingleCardPanels
                                    std::vector<shared::CardBase::id_t> selectedCardIds;
                                    std::vector<shared::ChooseFromOrder::AllowedChoice> allowedChoices(
                                            selectedCards.size(), allowed_choices);
                                    for ( auto &card : selectedCards ) {
                                        selectedCardIds.push_back(card->getCardName());
                                    }
                                    wxGetApp().getController().confirmSelectionFromHand(selectedCardIds,
                                                                                        allowedChoices);
                                    // clear the selected cards to be ready for the next selection
                                    selectedCards.clear();
                                });
            bool is_enabled = minCount == 0;
            confirmButton->Enable(is_enabled);
            DiscardPileSizer->Add(confirmButton, 0, wxALIGN_CENTER, 4);
        }


        // Add the discard pile to the sizer
        DiscardPileSizer->Add(DiscardPile, 0, wxALIGN_CENTER, 4);

        // Set the sizer for the discard pile panel
        DiscardPilePanel->SetSizer(DiscardPileSizer);

        return DiscardPilePanel;
    }

    void PlayerPanel::makeSelectable(SingleCardPanel *image)
    {
        image->SetToolTip("Select card");

        image->SetCursor(wxCursor(wxCURSOR_HAND));

        shared::CardBase::id_t card_id = image->getCardName();
        // Bind left click on the panel to the buyCard function
        image->makeClickable(wxEVT_LEFT_UP, [this, image](wxMouseEvent & /*event*/) { clickOnSelectableCard(image); });
    }

    void PlayerPanel::switchCardSelectionState(SingleCardPanel *card_panel)
    {
        // if the card is already selected remove it from the list and
        // thereby deselecting the card
        if ( std::ranges::count(selectedCards, card_panel) > 0 ) {
            auto it = std::ranges::find(selectedCards, card_panel);
            selectedCards.erase(it);
        } else {
            selectedCards.push_back(card_panel);
        }
    }

    void PlayerPanel::clickOnSelectableCard(SingleCardPanel *card_panel)
    {
        this->switchCardSelectionState(card_panel);
        LOG(INFO) << "Card " << card_panel->getCardName();

        bool is_selected = std::ranges::count(selectedCards, card_panel) > 0;
        unsigned int number_selected = selectedCards.size();

        // Change the border color of the card depending of the selection state
        wxColour new_border_colour = is_selected ? formatting_constants::SELECTED_CARD_BACKGROUND : wxNullColour;
        card_panel->setBorderColor(new_border_colour);

        // check if we should activate the confirm button
        if ( number_selected >= minCount && number_selected <= maxCount ) {
            confirmButton->Enable(true);
        } else {
            confirmButton->Enable(false);
        }
    }
    // NOLINTEND(bugprone-suspicious-enum-usage)
} // namespace client
