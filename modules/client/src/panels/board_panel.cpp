
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
#include <uiElements/popup.h>
#include "shared/game/cards/card_base.h"

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

        auto can_buy = [buy_phase, is_active](const shared::CardType /*type*/) { return buy_phase && is_active; };

        drawPiles(board, can_buy, treasure);
    }

    void BoardPanel::drawBoard(std::shared_ptr<shared::Board> board, unsigned int treasure, shared::CardType type)
    {
        this->DestroyChildren();

        auto can_buy = [type](const shared::CardType pile_type) { return (pile_type & type) == pile_type; };

        drawPiles(board, can_buy, treasure);
    }

    void BoardPanel::makeBuyable(PilePanel *pile)
    {
        // This display a little text box when hovering over the pile
        pile->SetToolTip("Buy card");

        pile->SetCursor(wxCursor(wxCURSOR_HAND));

        // Bind left click on the panel to the buyCard function
        pile->makeClickable(wxEVT_LEFT_UP,
                            [pile](wxMouseEvent & /*event*/)
                            { wxGetApp().getController().buyCard(pile->getPile().card_id); });
    }

    void BoardPanel::makePreview(PilePanel *pile)
    {
        pile->makeClickable(wxEVT_RIGHT_UP,
                            [pile](wxMouseEvent & /*event*/) { showCardPopup(pile, pile->getPile().card_id); });
    }

} // namespace client
