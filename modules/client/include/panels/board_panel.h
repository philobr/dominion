
#pragma once

#include <shared/game/cards/card_factory.h>
#include <shared/game/game_state/board_base.h>
#include <shared/game/game_state/game_phase.h>

#include <uiElements/formatting_constants.h>
#include <uiElements/pile_panel.h>
#include <wx/gbsizer.h>
#include <wx/wx.h>

namespace client
{
    /**
     * @brief The panel that draws the board
     * This specifically means the piles of cards and if necessary
     * the buttons to buy cards
     *
     */
    class BoardPanel : public wxPanel
    {
    public:
        /**
         * @brief Construct a new Board Panel object
         *
         * @param parent
         * @param size
         */
        BoardPanel(wxWindow *parent, wxSize size);

        /**
         * @brief Draw the board
         *
         * @param Board
         */
        void drawBoard(std::shared_ptr<shared::Board> board, bool is_active, unsigned int treasure,
                       shared::GamePhase phase);

    private:
        /**
         * @brief Make a pile buyable
         * by making it clickable and attaching an Event handler that sends a buy request
         */
        void makeBuyable(PilePanel *pile);

        /**
         * @brief Add the piles to the board placing them according to the given position rule
         * @param piles The piles from shared::Board to add
         * @param sizer The sizer to add the piles to
         * @param p The position rule to use must take unsigned int and return wxGBPosition
         * @param can_buy Whether the player can buy cards
         * @param treasure The amount of treasure the player has
         * @param panels The vector to store the created PilePanels in
         */
        template <typename PositionRule>
        void addPiles(const shared::Board::pile_container_t &piles, wxGridBagSizer *sizer, PositionRule p, bool can_buy,
                      int treasure, std::vector<PilePanel *> &panels);

        std::shared_ptr<shared::Board> board_;
        std::vector<PilePanel *> VictoryPiles_;
        std::vector<PilePanel *> TreasurePiles_;
        std::vector<PilePanel *> KingdomPiles_;
    };

    template <typename PositionRule>
    void BoardPanel::addPiles(const shared::Board::pile_container_t &piles, wxGridBagSizer *sizer, PositionRule p,
                              bool can_buy, int treasure, std::vector<PilePanel *> &panels)
    {
        unsigned int counter = 0;
        for ( const auto &pile : piles ) {
            PilePanel *Pile = new PilePanel(this, pile, formatting_constants::DEFAULT_BOARD_PILE_SIZE);
            panels.push_back(Pile);
            wxGBPosition position = p(counter);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;

            int price = shared::CardFactory::getCard(pile.card_id).getCost();
            if ( can_buy && price <= treasure ) {
                makeBuyable(Pile);
            }
        }
    }

} // namespace client
