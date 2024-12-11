#include <panels/phase_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/single_card_panel.h>

#include <dominion.h>

using shared::GamePhase;

namespace client
{
    PhaseInfoPanel::PhaseInfoPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        if ( wxGetApp().isDebugMode() ) {
            LOG(WARN) << "using hard coded game_state for testing";
            auto player = shared::PlayerBase("gigu");
            auto reduced = reduced::Player::make(player, {"Village", "Copper", "Copper"});
            auto board = shared::Board::make({"Council_Room", "Smithy", "Village", "Laboratory", "Festival", "Market",
                                              "Gardens", "Duke", "Silk_Road", "Witch"},
                                             2);
            auto game_state = std::make_unique<reduced::GameState>(
                    board, std::move(reduced), std::vector<reduced::Enemy::ptr_t>(), "gigu", GamePhase::ACTION_PHASE);

            drawInfoPanel(*game_state);
        }
    }

    void PhaseInfoPanel::drawInfoPanel(const reduced::GameState &game_state)
    {
        this->DestroyChildren();

        // Adjust background colour based on active player
        if ( game_state.isPlayerActive() ) {
            SetBackgroundColour(formatting_constants::ACTIVE_PLAYER_INFO_BACKGROUND);
        } else {
            SetBackgroundColour(formatting_constants::PASSIVE_PLAYER_INFO_BACKGROUND);
        }

        // Create a grid sizer for the panel
        wxGridSizer *sizer = new wxGridSizer(1, 3, 0, 0);


        // Add player info to the sizer
        if ( game_state.active_player == game_state.reduced_player->getId() ) {
            // if the current player is the active player
            sizer->Add(drawPlayerInfo(*game_state.reduced_player),
                       wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL).Border(wxALL, 2));
        } else {
            // if the current player is not the active player, then the stats of the playing enemy are shown
            for ( const auto &enemy : game_state.reduced_enemies ) {
                if ( enemy->getId() == game_state.active_player ) {
                    sizer->Add(drawPlayerInfo(*enemy),
                               wxSizerFlags().Align(wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL).Border(wxALL, 2));
                    break;
                }
            }
        }

        // Add played cards to the sizer
        auto *playedPanel = drawPlayedPanel(game_state.board->getPlayedCards());

        // Add buttons to the sizer
        auto *buttonsPanel = drawButtonPanel(game_state);

        // Add the panels to the sizer
        sizer->Add(playedPanel, wxSizerFlags().Align(wxALIGN_CENTER).Border(wxALL, 5));
        sizer->Add(buttonsPanel,
                   wxSizerFlags().Align(wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL).Border(wxRIGHT | wxLEFT, 5));

        // Set minimum width for the info bar
        SetMinSize(wxSize(150, -1)); // 150 pixels wide, height automatic

        this->SetSizer(sizer);
        this->Layout();
    }

    TextPanel *PhaseInfoPanel::drawPlayerInfo(const shared::PlayerBase &player)
    {
#ifdef __APPLE__
        wxString info = wxString::Format("Currently playing: %s\n\nTreasure: %d\n\nActions: %d\n\nBuys: %d",
                                         player.getId(), player.getTreasure(), player.getActions(), player.getBuys());
        return new TextPanel(this, wxID_ANY, info, TextFormat::BOLD);
#else
        wxString info = wxString::Format("Currently playing: %s\nTreasure: %d\nActions: %d\nBuys: %d", player.getId(),
                                         player.getTreasure(), player.getActions(), player.getBuys());
        return new TextPanel(this, wxID_ANY, info, TextFormat::BOLD_SMALL);
#endif
    }

    wxPanel *PhaseInfoPanel::drawPlayedPanel(const std::vector<shared::CardBase::id_t> cards)
    {
        // Get the hand cards
        size_t num_cards = cards.size();
        size_t card_width_borders = formatting_constants::DEFAULT_PLAYED_CARD_SIZE.GetWidth() + 8;

        wxScrolledWindow *scrolledWindow = new wxScrolledWindow(this, wxID_ANY, wxDefaultPosition,
                                                                wxSize(6 * card_width_borders + 30, 150), wxHSCROLL);
        scrolledWindow->SetScrollRate(10, 0); // Set scroll rate in pixels

        // Create the sizer for the hand
        wxGridSizer *sizer = new wxGridSizer(1, num_cards, 5, 5);
        sizer->SetMinSize(wxSize(num_cards * card_width_borders, 150));

        // Add the cards to the hand
        for ( size_t i = 0; i < num_cards; i++ ) {
            SingleCardPanel *card =
                    new SingleCardPanel(scrolledWindow, cards[i], formatting_constants::DEFAULT_PLAYED_CARD_SIZE);

            sizer->Add(card, 0, wxALL, 4);
        }

        // Set the sizer for the hand panel
        scrolledWindow->SetSizer(sizer);
        scrolledWindow->FitInside();
        return scrolledWindow;
    }

    wxButton *PhaseInfoPanel::createEndActionButton(wxWindow *parent)
    {
        wxButton *endActionPhaseButton =
                new wxButton(parent, wxID_ANY, "End Action", wxDefaultPosition, wxSize(100, 40));
        endActionPhaseButton->Bind(
                wxEVT_BUTTON, [](const wxCommandEvent & /*event*/) { wxGetApp().getController().endActionPhase(); });
        return endActionPhaseButton;
    }

    wxButton *PhaseInfoPanel::createEndTurnButton(wxWindow *parent)
    {
        wxButton *endTurnButton = new wxButton(parent, wxID_ANY, "End Turn", wxDefaultPosition, wxSize(100, 40));
        endTurnButton->Bind(wxEVT_BUTTON,
                            [](const wxCommandEvent & /*event*/) { wxGetApp().getController().endTurn(); });
        return endTurnButton;
    }

    TextPanel *PhaseInfoPanel::drawPhaseInfo(wxWindow *parent, const std::string &game_phase)
    {
        wxString info = wxString::Format("Current phase: %s", game_phase);
#ifdef __APPLE__
        return new TextPanel(parent, wxID_ANY, info, TextFormat::BOLD);
#else
        return new TextPanel(parent, wxID_ANY, info, TextFormat::BOLD_SMALL);
#endif
    }

    // NOLINTBEGIN(bugprone-suspicious-enum-usage)
    wxPanel *PhaseInfoPanel::drawButtonPanel(const reduced::GameState &game_state)
    {
        // Create a container panel for the buttons
        wxPanel *buttonPanel = new wxPanel(this, wxID_ANY);

        // Create a vertical sizer for the buttons
        wxBoxSizer *verticalSizer = new wxBoxSizer(wxVERTICAL);

        verticalSizer->Add(drawPhaseInfo(buttonPanel, shared::gamePhaseToDisplayedString(game_state.game_phase)), 0,
                           wxALIGN_CENTER | wxRIGHT | wxLEFT, 5);

        if ( game_state.reduced_player->getId() == game_state.active_player ) {
            if ( game_state.game_phase == GamePhase::ACTION_PHASE ) {
                wxButton *endActionPhaseButton = createEndActionButton(buttonPanel);
                verticalSizer->Add(endActionPhaseButton, 0, wxCENTER | wxALL, 5);
            }
            if ( game_state.game_phase == GamePhase::BUY_PHASE || game_state.game_phase == GamePhase::ACTION_PHASE ) {
                wxButton *endTurnButton = createEndTurnButton(buttonPanel);
                verticalSizer->Add(endTurnButton, 0, wxCENTER | wxALL, 5);
            }
        }

        // Set the sizer for the panel
        buttonPanel->SetSizer(verticalSizer);

        return buttonPanel;
    }
    // NOLINTEND(bugprone-suspicious-enum-usage)

} // namespace client
