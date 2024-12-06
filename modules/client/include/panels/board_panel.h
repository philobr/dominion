
#pragma once

#include <shared/game/cards/card_factory.h>
#include <shared/game/game_state/board_base.h>
#include <shared/game/game_state/game_phase.h>
#include <shared/game/cards/card_base.h>


#include <uiElements/formatting_constants.h>
#include <uiElements/pile_panel.h>
#include <wx/gbsizer.h>

#include <shared/utils/logger.h>

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
         * @brief Draw the board in standard game phase
         *
         * @param Board
         */
        void drawBoard(std::shared_ptr<shared::Board> board, bool is_active, unsigned int treasure,
                       shared::GamePhase phase);

        /**
         * @brief Draw the board in a GainFromBoardPhase
         * e.g when Remodel is played type will be any type or if Mine is played type will be Treasure
         *
         */
        void drawBoard(std::shared_ptr<shared::Board> board, unsigned int treasure, shared::CardType type);

    private:

        /**
         * @brief Make a pile buyable
         * by making it clickable and attaching an Event handler that sends a buy request
         */
        void makeBuyable(PilePanel *pile);

        /**
         * @brief Draw the piles of the board
         * @param board The board to draw
         * @param can_select A function that takes a CardType and returns whether the player can select that type of
         * card
         * @param treasure The maximum amount of treasure a card can have to be made buyable
         */
        template <typename TypeRule>
        void drawPiles(std::shared_ptr<shared::Board> board, TypeRule can_select, int treasure);

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

    template <typename TypeRule>
    void BoardPanel::drawPiles(std::shared_ptr<shared::Board> board, TypeRule can_buy, int treasure)
    {

        const auto &VictoryCards = board->getVictoryCards();
        const auto &TreasureCards = board->getTreasureCards();
        const auto &KingdomCards = board->getKingdomCards();

        // use a grid bag sizer that allow us to place the cards in a grid
        // and not fill all spaces
        // In the future this even allows for uneven sizes where some panels can
        // take up multiple grid spaces
        auto *sizer = new wxGridBagSizer(10, 10);

        // places the victory piles all the way to the left in one column
        auto VictoryPositionRule = [](unsigned int counter) { return wxGBPosition(counter, 0); };

        // places the treasure piles next to the victory piles in one column
        auto TreasurePositionRule = [](unsigned int counter) { return wxGBPosition(counter, 1); };

        // places the kingdom piles in a 2x5 grid
        auto KingdomPositionRule = [](unsigned int counter) { return wxGBPosition(counter % 2, 2 + counter / 2); };


        addPiles(VictoryCards, sizer, VictoryPositionRule, can_buy(shared::CardType::VICTORY), treasure, VictoryPiles_);
        addPiles(TreasureCards, sizer, TreasurePositionRule, can_buy(shared::CardType::TREASURE), treasure,
                 TreasurePiles_);
        addPiles(KingdomCards, sizer, KingdomPositionRule, can_buy(shared::CardType::KINGDOM), treasure, KingdomPiles_);

        // necessary command for the grid bag sizer to do it's thing and arrange
        // the panels
        this->SetSizer(sizer, true);
        sizer->Layout();
    }
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
            } else {
                Pile->SetToolTip("Too expensive");
                Pile->makeGrey();
                Pile->SetCursor(wxCursor(wxCURSOR_NO_ENTRY));
                
            }
        }
    }


} // namespace client
