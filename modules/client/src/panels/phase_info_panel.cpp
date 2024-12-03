#include <panels/phase_info_panel.h>
#include <shared/utils/logger.h>


namespace client
{
    PhaseInfoPanel::PhaseInfoPanel(wxWindow* parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        LOG(WARN) << "using hard coded game_state for testing";
        auto player = shared::PlayerBase("gigu");
        auto reduced = reduced::Player::make(player, { "Village", "Copper", "Copper", "Copper", "Estate" });
        auto game_state = std::make_unique<reduced::GameState>(nullptr, std::move(reduced), std::vector<reduced::Enemy::ptr_t>(), "gigu");

        // Set background color to light blue
        SetBackgroundColour(wxColour(200, 220, 240));

        this->drawInfoPanel(game_state);
    }

    void drawInfoPanel(const reduced::Player::ptr_t& player, const std::vector<reduced::Enemy::ptr_t>& enemies, const shared::PlayerBase::id_t& active_player);
    {
        this->DestroyChildren();

        // Create vertical BoxSizer
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        // Add player info to the sizer
        auto* infoPanel = drawPlayerInfo(game_state->reduced_player);
        sizer->Add(infoPanel, 0, wxALL, 5);

        //
        // Set minimum width for the info bar
        SetMinSize(wxSize(150, -1));  // 150 pixels wide, height automatic

        this->SetSizer(sizer);
        sizer->Layout();
    }

    TextPanel* PhaseInfoPanel::drawPlayerInfo(const std::unique_ptr<reduced::Player>& player)
    {
        wxString info = wxString::Format(
            "%s\nGold: %d\nActions: %d\nBuys: %d",
            player->getId(),
            player->getTreasure(),
            player->getActions(),
            player->getBuys()
        );

        return new TextPanel(this, wxID_ANY, info);
    }

    // wxPanel* PhaseInfoPanel::drawPlayedPanel(const std::vector<shared::CardBase::id_t> cards)
    // {
    //     // Get the hand cards
    //     size_t hand_size = cards.size();

    //     // Create the hand panel
    //     wxPanel* hand = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);

    //     // Create the sizer for the hand
    //     wxGridSizer* sizer = new wxGridSizer(1, hand_size, 0, 0);
    //     sizer->SetMinSize(wxSize(5 * card_width_borders, 150));

    //     // Set the sizer for the hand panel
    //     hand->SetSizer(sizer);

    //     // Set the size of the cards
    //     if (card_width_borders * hand_size > 724) {
    //         // scale bigger hands
    //         hand_card_size.SetWidth(724 / hand_size - 8);
    //         hand_card_size.SetHeight(hand_card_size.GetWidth() / 4 * 5);
    //     }

    //     // Add the cards to the hand
    //     for (size_t i = 0; i < hand_size; i++) {
    //         ImagePanel* card = new ImagePanel(hand, "assets/" + cards[i] + ".png", wxBITMAP_TYPE_PNG, wxDefaultPosition,
    //             hand_card_size, 0);

    //         bool is_action = shared::CardFactory::getCard(cards[i]).isAction();

    //         if (is_action && is_active && player->getActions() > 0) {
    //             makePlayable(card, cards[i]);
    //         }

    //         sizer->Add(card, 0, wxALIGN_CENTER, 4);
    //     }

    //     // Set the sizer for the hand panel
    //     sizer->Layout();

    //     return hand;
    // }
} // namespace client
