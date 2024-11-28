#include <panels/board_panel.h>

#include <game_controller.h>
#include <shared/utils/logger.h>
#include <uiElements/pile_panel.h>
#include <wx/gbsizer.h>
#include <wx/wx.h>

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


    void BoardPanel::drawBoard(std::shared_ptr<shared::Board> Board, bool is_active, unsigned int treasure)
    {
        // this->DestroyChildren();
        LOG(INFO) << "Destroyed Children";

        // board_ = Board;

        const auto &VictoryCards = Board->getVictoryCards();
        const auto &TreasureCards = Board->getTreasureCards();
        const auto &KingdomCards = Board->getKingdomCards();
        LOG(INFO) << "Got Cards";

        // use a grid bag sizer that allow us to place the cards in a grid
        // and not fill all spaces
        // In the future this even allows for uneven sizes where some panels can
        // take up multiple grid spaces
        auto *sizer = new wxGridBagSizer(10, 10);
        LOG(INFO) << "Set sizer";
        unsigned int counter = 0;
        for ( const auto &VictoryPile : VictoryCards ) {
            PilePanel *Pile = new PilePanel(this, VictoryPile, formatting_constants::DEFAULT_BOARD_PILE_SIZE);
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
        LOG(INFO) << "Drew VictoryCards";

        counter = 0;
        for ( const auto &TreasurePile : TreasureCards ) {
            PilePanel *Pile = new PilePanel(this, TreasurePile, formatting_constants::DEFAULT_BOARD_PILE_SIZE);
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
        LOG(INFO) << "Drew TreasureCards";

        counter = 0;
        for ( const auto &KingdomPile : KingdomCards ) {
            LOG(INFO) << "New loop: " << counter << " " << KingdomPile.card_id;
            PilePanel *Pile = new PilePanel(this, KingdomPile, formatting_constants::DEFAULT_BOARD_PILE_SIZE));
            LOG(INFO) << "Created Pile";

            wxGBPosition position = wxGBPosition(counter % 2, 2 + counter / 2);
            LOG(INFO) << "Got Position";
            wxGBSpan span = wxGBSpan(1, 1);
            LOG(INFO) << "Set span";

            // TODO get this logic out of GUI maybe create some utils functions?
            unsigned int price = shared::CardFactory::getCard(KingdomPile.card_id).getCost();
            LOG(INFO) << "Got price";
            // check buyability
            if ( is_active && price <= treasure ) {
                LOG(INFO) << "Is buyable";
                makeBuyable(Pile);
                LOG(INFO) << "Made buyable";
            }
            LOG(INFO) << "Checked if buyable";

            sizer->Add(Pile, position, span, wxALIGN_CENTER_HORIZONTAL);
            LOG(INFO) << "Added to sizer";
            counter++;
            LOG(INFO) << "Incremented counter";
        }
        LOG(INFO) << "Drew KingdomCards";

        wxButton *EndTurnButton = new wxButton(this, wxID_ANY, "End Turn");
        EndTurnButton->Bind(wxEVT_BUTTON, [](const wxCommandEvent &) { GameController::endTurn(); });
        LOG(INFO) << "Bound EndTurnButton";

        wxGBPosition position = wxGBPosition(3, 9);
        wxGBSpan span = wxGBSpan(1, 1);
        sizer->Add(EndTurnButton, position, span, wxALIGN_CENTER_HORIZONTAL);
        LOG(INFO) << "Added EndTurnButton";

        // necessary command for the grid bag sizer to do it's thing and arrange
        // the panels
        sizer->Layout();
        LOG(INFO) << "Done with sizer->Layout()";
        this->SetSizer(sizer, true);
        LOG(INFO) << "Done with this->SetSizer()";
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
