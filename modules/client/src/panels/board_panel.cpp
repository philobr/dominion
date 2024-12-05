
#include <panels/board_panel.h>

#include <game_controller.h>
#include <shared/utils/logger.h>
#include <uiElements/pile_panel.h>
#include <wx/gbsizer.h>
#include <wx/wx.h>

#include <dominion.h>
#include <shared/game/cards/card_factory.h>
#include <shared/utils/test_helpers.h>
#include <uiElements/formatting_constants.h>

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {

        auto board = shared::Board::make(getValidKingdomCards(), 3);

        this->drawBoard(board, true, 3, shared::GamePhase::ACTION_PHASE);
    }

    void BoardPanel::drawBoard(std::shared_ptr<shared::Board> board, bool is_active, unsigned int treasure,
                               shared::GamePhase phase)
    {
        this->DestroyChildren();

        bool buy_phase = phase == shared::GamePhase::BUY_PHASE;

        // board_ = Board;

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

        bool canBuy = is_active && buy_phase;
        addPiles(VictoryCards, sizer, VictoryPositionRule, canBuy, treasure, VictoryPiles_);
        addPiles(TreasureCards, sizer, TreasurePositionRule, canBuy, treasure, TreasurePiles_);
        addPiles(KingdomCards, sizer, KingdomPositionRule, canBuy, treasure, KingdomPiles_);

        // necessary command for the grid bag sizer to do it's thing and arrange
        // the panels
        this->SetSizer(sizer, true);
        sizer->Layout();
    }

    void BoardPanel::makeBuyable(PilePanel *pile)
    {
        // This display a little text box when hovering over the pile
        pile->SetToolTip("Buy card");

        pile->SetCursor(wxCursor(wxCURSOR_HAND));

        // Bind left click on the panel to the buyCard function
        pile->makeClickable([pile](wxMouseEvent & /*event*/)
                            { wxGetApp().getController().buyCard(pile->getPile().card_id); });
    }


} // namespace client
