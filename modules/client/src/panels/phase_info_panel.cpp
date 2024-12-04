#include <panels/phase_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/single_card_panel.h>

#include <dominion.h>

namespace client
{
    PhaseInfoPanel::PhaseInfoPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        LOG(WARN) << "using hard coded game_state for testing";
        auto player = shared::PlayerBase("gigu");
        auto reduced = reduced::Player::make(player, {"Village", "Copper", "Copper"});
        auto board = shared::Board::make({"Moat", "Smithy", "Village", "Laboratory", "Festival", "Market",
                                          "Placeholder1", "Placeholder2", "Placeholder3", "Placeholder4"},
                                         2);
        auto game_state =
                std::make_unique<reduced::GameState>(board, std::move(reduced), std::vector<reduced::Enemy::ptr_t>(),
                                                     "gigu", shared::GamePhase::ACTION_PHASE);

        // Set background color to light blue
        SetBackgroundColour(formatting_constants::PLAYER_INFO_BACKGROUND);

        drawInfoPanel(*game_state);
    }

    void PhaseInfoPanel::drawInfoPanel(const reduced::GameState &game_state)
    {
        this->DestroyChildren();

        // Create a grid sizer for the panel
        wxGridSizer *sizer = new wxGridSizer(1, 3, 0, 10);

        // Add player info to the sizer
        auto *infoPanel = drawPlayerInfo(game_state.reduced_player);

        // Add played cards to the sizer
        auto *playedPanel = drawPlayedPanel(game_state.board->getPlayedCards());

        // Add buttons to the sizer
        auto *buttonsPanel = drawButtonPanel();

        // Add the panels to the sizer
        sizer->Add(infoPanel, wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));
        sizer->Add(playedPanel, wxSizerFlags().Align(wxALIGN_CENTER).Border(wxALL, 5));
        sizer->Add(buttonsPanel, wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Border(wxALL, 5));

        // Set minimum width for the info bar
        SetMinSize(wxSize(150, -1)); // 150 pixels wide, height automatic

        this->SetSizer(sizer);
        this->Layout();
    }

    TextPanel *PhaseInfoPanel::drawPlayerInfo(const std::unique_ptr<reduced::Player> &player)
    {
        wxString info = wxString::Format("%s\n\nTreasure: %d\n\nActions: %d\n\nBuys: %d", player->getId(),
                                         player->getTreasure(), player->getActions(), player->getBuys());

        return new TextPanel(this, wxID_ANY, info, TextFormat::BOLD);
    }

    wxPanel *PhaseInfoPanel::drawPlayedPanel(const std::vector<shared::CardBase::id_t> cards)
    {
        // Get the hand cards
        size_t cards_size = cards.size();
        size_t card_width_borders = formatting_constants::DEFAULT_PLAYED_CARD_SIZE.GetWidth() + 8;
        const size_t num_cards = std::min(cards_size, (size_t)5);

        // Create the hand panel
        wxPanel *hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

        // Create the sizer for the hand
        wxGridSizer *sizer = new wxGridSizer(1, num_cards, 5, 5);
        sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

        const size_t start_idx = cards_size - num_cards;
        // Add the cards to the hand
        for ( size_t i = start_idx; i < std::max(num_cards, cards_size); i++ ) {
            SingleCardPanel *card = new SingleCardPanel(hand, cards[i], formatting_constants::DEFAULT_PLAYED_CARD_SIZE);

            sizer->Add(card, 0, wxALL, 4);
        }

        // Set the sizer for the hand panel
        hand->SetSizer(sizer);
        return hand;
    }

    wxButton *PhaseInfoPanel::getEndActionButton()
    {
        wxButton *endActionPhaseButton = new wxButton(this, wxID_ANY, "End Action", wxDefaultPosition, wxSize(100, 40));
        endActionPhaseButton->Bind(
                wxEVT_BUTTON, [](const wxCommandEvent & /*event*/) { wxGetApp().getController().endActionPhase(); });
        return endActionPhaseButton;
    }

    wxButton *PhaseInfoPanel::getEndTurnButton()
    {
        wxButton *endTurnButton = new wxButton(this, wxID_ANY, "End Turn", wxDefaultPosition, wxSize(100, 40));
        endTurnButton->Bind(wxEVT_BUTTON,
                            [](const wxCommandEvent & /*event*/) { wxGetApp().getController().endTurn(); });
        return endTurnButton;
    }

    wxPanel *PhaseInfoPanel::drawButtonPanel()
    {
        // Create a container panel for the buttons
        wxPanel *buttonPanel = new wxPanel(this, wxID_ANY);

        // Create a vertical sizer for the buttons
        wxBoxSizer *verticalSizer = new wxBoxSizer(wxVERTICAL);

        // Get the buttons using existing functions
        wxButton *endActionPhaseButton = getEndActionButton();
        wxButton *endTurnButton = getEndTurnButton();

        // Reparent the buttons to the new panel
        endActionPhaseButton->Reparent(buttonPanel);
        endTurnButton->Reparent(buttonPanel);

        // Add buttons to the vertical sizer with some spacing
        verticalSizer->Add(endActionPhaseButton, 0, wxALL, 5);
        verticalSizer->Add(endTurnButton, 0, wxALL, 5);

        // Set the sizer for the panel
        buttonPanel->SetSizer(verticalSizer);

        return buttonPanel;
    }

} // namespace client
