#pragma once

#include <shared/game/game_state/game_phase.h>
#include <shared/game/reduced/player.h>

#include <wx/wx.h>

#include <uiElements/single_card_panel.h>

#include <algorithm>
#include <ranges>

namespace client
{
    /**
     * @brief The panel that displays the information about the player
     * This includes their hand cards, deck size, discard pile size and
     * the number of actions, buys and treasure they have left.
     */
    class PlayerPanel : public wxPanel
    {
    public:
        PlayerPanel(wxWindow *parent, wxSize size);

        void drawPlayer(const std::unique_ptr<reduced::Player> &player, bool is_active, shared::GamePhase phase);

    private:
        void makePlayable(SingleCardPanel *image, const std::string &card_id);

        wxSize hand_card_size = wxSize(100, 125);

        /**
         * @brief Create the draw pile panel
         * @param draw_pile_size
         *
         * @return wxPanel*
         */
        wxPanel *createDrawPilePanel(const unsigned int draw_pile_size);

        /**
         * @brief Create the hand panel
         *
         * @param std::unique_ptr<reduced::Player>& Player
         * @param size_t card_width_borders
         * @param bool is_active
         *
         * @return wxPanel*
         */
        wxPanel *createHandPanel(const std::unique_ptr<reduced::Player> &player, const size_t card_width_borders,
                                 const bool is_active, shared::GamePhase phase);

        /**
         * @brief Create the discard pile panel
         * @param  discard_pile_size
         * @param  top_discard_card
         * @return wxPanel*
         */
        wxPanel *createDiscardPilePanel(const unsigned int discard_pile_size, const std::string &top_discard_card);

        /**
         * @brief make a card panel selectable
         */
        void makeSelectable(SingleCardPanel *card_panel);

        /**
         * @brief togle the seleced state by erasing or adding to the list of selected cards
         */
        void switchCardSelectionState(shared::CardBase::id_t card_id);

        // vector that is used when we are in SelectFromHand Phase
        std::vector<shared::CardBase::id_t> selectedCards;
    };

} // namespace client
