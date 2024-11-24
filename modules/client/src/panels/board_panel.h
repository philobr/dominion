#pragma once

#include <shared/game/game_state/board_base.h>

#include <wx/wx.h>
#include "pile_panel.h"

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
        void DrawBoard(std::shared_ptr<shared::Board> Board, bool is_active, unsigned int treasure);

    private:
        /**
         * @brief Make a pile buyable
         * by making it clickable and attaching an Event handler that sends a buy request
         */
        void makeBuyable(PilePanel *Pile);

        std::shared_ptr<shared::Board> board_;
    };

} // namespace client
