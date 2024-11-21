#include "board_panel.h"

#include "../game_controller.h"
#include <wx/gbsizer.h>
#include <wx/wx.h>
#include "pile_panel.h"

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

        const auto &VictoryCards = Board->getVictoryCards();
        const auto &TreasureCards = Board->getTreasureCards();
        const auto &KingdomCards = Board->getKingdomCards();

        auto *sizer = new wxGridBagSizer(10, 10);
        unsigned int counter = 0;
        for ( const auto &VictoryPile : VictoryCards ) {
            PilePanel *Pile = new PilePanel(this, VictoryPile);
            wxGBPosition position = wxGBPosition(counter, 0);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);

            if (is_active) {
                makeBuyable(Pile);
            }

            counter++;
        }

        counter = 0;
        for ( const auto &TreasurePile : TreasureCards ) {
            PilePanel *Pile = new PilePanel(this, TreasurePile);
            wxGBPosition position = wxGBPosition(counter, 1);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            
            if (is_active) {
                makeBuyable(Pile);
            }

            counter++;
        }

        counter = 0;
        for ( const auto &KingdomPile : KingdomCards ) {
            PilePanel *Pile = new PilePanel(this, KingdomPile);
            wxGBPosition position = wxGBPosition(counter % 2, 2 + counter / 2);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);

            if (is_active) {
                makeBuyable(Pile);
            }

            counter++;
        }
        sizer->Layout();
        this->SetSizer(sizer, true);
    }

    void BoardPanel::makeBuyable(PilePanel* Pile)
    {
        Pile->SetToolTip("Buy card");
        Pile->SetCursor(wxCursor(wxCURSOR_HAND));
        Pile->Bind(wxEVT_LEFT_UP, [&Pile](wxMouseEvent& /*event*/) {
            GameController::buyCard(Pile->getPile().card_id);
        });
    }


} // namespace client
