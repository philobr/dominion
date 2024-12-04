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

        this->drawBoard(board, true, 3);
    }


    void BoardPanel::drawBoard(std::shared_ptr<shared::Board> board, bool is_active, unsigned int treasure)
    {
        this->DestroyChildren();

        // board_ = Board;

        const auto &VictoryCards = board->getVictoryCards();
        const auto &TreasureCards = board->getTreasureCards();
        const auto &KingdomCards = board->getKingdomCards();

        // use a grid bag sizer that allow us to place the cards in a grid
        // and not fill all spaces
        // In the future this even allows for uneven sizes where some panels can
        // take up multiple grid spaces
        auto *sizer = new wxGridBagSizer(10, 10);
        unsigned int counter = 0;
        for ( const auto &VictoryPile : VictoryCards ) {
            PilePanel *Pile = new PilePanel(this, VictoryPile, formatting_constants::DEFAULT_BOARD_PILE_SIZE);
            wxGBPosition position = wxGBPosition(counter, 0);
            wxGBSpan span = wxGBSpan(1, 1);

            checkIfBuyable(Pile, VictoryPile.card_id, is_active, treasure);

            // adding the grid bag sizer requires the panel to add as well as its
            // grid position and span meaning how many grid squares it should
            // take up in x,y direction
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;
        }

        counter = 0;
        for ( const auto &TreasurePile : TreasureCards ) {
            PilePanel *Pile = new PilePanel(this, TreasurePile, formatting_constants::DEFAULT_BOARD_PILE_SIZE);
            wxGBPosition position = wxGBPosition(counter, 1);
            wxGBSpan span = wxGBSpan(1, 1);

            checkIfBuyable(Pile, TreasurePile.card_id, is_active, treasure);

            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;
        }

        counter = 0;
        for ( const auto &KingdomPile : KingdomCards ) {
            PilePanel *Pile = new PilePanel(this, KingdomPile, formatting_constants::DEFAULT_BOARD_PILE_SIZE);

            wxGBPosition position = wxGBPosition(counter % 2, 2 + counter / 2);
            wxGBSpan span = wxGBSpan(1, 1);

            checkIfBuyable(Pile, KingdomPile.card_id, is_active, treasure);

            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;
        }

        // necessary command for the grid bag sizer to do it's thing and arrange
        // the panels
        this->SetSizer(sizer, true);
        sizer->Layout();
    }

    void BoardPanel::checkIfBuyable(PilePanel *pile, shared::CardBase::id_t card_id, bool is_active, unsigned int treasure)
    {
        unsigned int price = shared::CardFactory::getCard(card_id).getCost();
        if (is_active && price <= treasure) {
            LOG(INFO) << "Making buyable";
            makeBuyable(pile);
        } else {
            LOG(INFO) << "Making not buyable";
            pile->greyOut();
        }

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
