#include "board_panel.h"

#include <wx/gbsizer.h>
#include <wx/wx.h>
#include "../game_controller.h"
#include "pile_panel.h"

#include <shared/game/cards/card_factory.h>
#include <shared/utils/test_helpers.h>

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {

        auto board = shared::Board::make(get_valid_kingdom_cards(), 3);

        this->DrawBoard(board, true, 3);
    }


    void BoardPanel::DrawBoard(std::shared_ptr<shared::Board> Board, bool is_active, unsigned int treasure)
    {
        this->DestroyChildren();

        // board_ = Board;

        const auto &VictoryCards = Board->getVictoryCards();
        const auto &TreasureCards = Board->getTreasureCards();
        const auto &KingdomCards = Board->getKingdomCards();

        // use a grid bag sizer that allow us to place the cards in a grid
        // and not fill all spaces
        // In the future this even allows for uneven sizes where some panels can
        // take up multiple grid spaces
        auto *sizer = new wxGridBagSizer(10, 10);
        unsigned int counter = 0;
        for ( const auto &VictoryPile : VictoryCards ) {
            PilePanel *Pile = new PilePanel(this, VictoryPile);
            wxGBPosition position = wxGBPosition(counter, 0);
            wxGBSpan span = wxGBSpan(1, 1);

            // TODO get this logic out of GUI maybe create some utils functions?
            unsigned int price = shared::CardFactory::getCard(VictoryPile.card_id).getCost();
            // check buyability
            if ( is_active && price <= treasure ) {
                makeBuyable(Pile);
            }

            // adding the grid bag sizer requires the panel to add as well as its
            // grid position and span meaning how many grid squares it should
            // take up in x,y direction
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;
        }

        counter = 0;
        for ( const auto &TreasurePile : TreasureCards ) {
            PilePanel *Pile = new PilePanel(this, TreasurePile);
            wxGBPosition position = wxGBPosition(counter, 1);
            wxGBSpan span = wxGBSpan(1, 1);

            // TODO get this logic out of GUI maybe create some utils functions?
            unsigned int price = shared::CardFactory::getCard(TreasurePile.card_id).getCost();
            // check buyability
            if ( is_active && price <= treasure ) {
                makeBuyable(Pile);
            }

            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;
        }

        counter = 0;
        for ( const auto &KingdomPile : KingdomCards ) {
            PilePanel *Pile = new PilePanel(this, KingdomPile);
            wxGBPosition position = wxGBPosition(counter % 2, 2 + counter / 2);
            wxGBSpan span = wxGBSpan(1, 1);

            // TODO get this logic out of GUI maybe create some utils functions?
            unsigned int price = shared::CardFactory::getCard(KingdomPile.card_id).getCost();
            // check buyability
            if ( is_active && price <= treasure ) {
                makeBuyable(Pile);
            }

            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            counter++;
        }

        // necessary command for the grid bag sizer to do it's thing and arrange
        // the panels
        sizer->Layout();
        this->SetSizer(sizer, true);
    }

    void BoardPanel::makeBuyable(PilePanel *Pile)
    {
        // This display a little text box when hovering over the pile
        Pile->SetToolTip("Buy card");

        Pile->SetCursor(wxCursor(wxCURSOR_HAND));

        // Bind left click on the panel to the buyCard function
        Pile->makeClickable([Pile](wxMouseEvent & /*event*/) { GameController::buyCard(Pile->getPile().card_id); });
    }


} // namespace client
