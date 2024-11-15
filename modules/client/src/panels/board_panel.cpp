#include "board_panel.h"
#include <wx/gbsizer.h>
#include <wx/wx.h>
#include "pile_panel.h"

namespace client
{

    BoardPanel::BoardPanel(wxWindow *parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {

        shared::Board board;
        board.initialise_treasure_cards(2);
        board.initialise_victory_cards(2);
        board.kingdom_cards = {{"Chapel", 8}, {"Village", 8}, {"Laboratory", 8}, {"Artisan", 8}};
        this->DrawBoard(board);
    }


    void BoardPanel::DrawBoard(shared::Board &Board)
    {
        this->DestroyChildren();

        auto *sizer = new wxGridBagSizer(10, 10);
        for ( unsigned i = 0; i < Board.victory_cards.size(); i++ ) {
            PilePanel *Pile = new PilePanel(this, Board.victory_cards[i]);
            wxGBPosition position = wxGBPosition(i, 0);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
        }
        for ( unsigned i = 0; i < Board.treasure_cards.size(); i++ ) {
            PilePanel *Pile = new PilePanel(this, Board.treasure_cards[i]);
            wxGBPosition position = wxGBPosition(i, 1);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
        }
        for ( unsigned i = 0; i < Board.kingdom_cards.size(); i++ ) {
            PilePanel *Pile = new PilePanel(this, Board.kingdom_cards[i]);
            wxGBPosition position = wxGBPosition(i % 2, 2 + i / 2);
            wxGBSpan span = wxGBSpan(1, 1);
            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
        }
        sizer->Layout();
        this->SetSizer(sizer, true);
    }


} // namespace client
