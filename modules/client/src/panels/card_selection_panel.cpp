#include <panels/card_selection_panel.h>

#include <game_controller.h>
#include <uiElements/formatting_constants.h>
#include <uiElements/image_panel.h>
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>
#include "dominion.h"
#include "shared/game/cards/card_base.h"
#include "shared/game/cards/card_factory.h"
#include "shared/utils/logger.h"
#include "uiElements/text_panel.h"

namespace client
{
    CardSelectionPanel::CardSelectionPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024))
    {
        TextPanel *Title = new TextPanel(this, wxID_ANY, "Card Selection", TextFormat::TITLE);
        wxBoxSizer *VerticalSizer = new wxBoxSizer(wxVERTICAL);
        VerticalSizer->Add(Title, 0, wxALIGN_CENTER | wxALL, 5);

        const shared::CardFactory::map_t &all_cards = shared::CardFactory::getAll();
        for ( const auto &card : all_cards ) {
            if ( card.second->isAction() ) {
                selectedCards[card.first] = false;
            }
        }

        VerticalSizer->Add(createCardSelection(), 0, wxALIGN_CENTER | wxALL, 5);

        this->SetSizerAndFit(VerticalSizer);
    }

    wxGridSizer *CardSelectionPanel::createCardSelection()
    {
        wxGridSizer *CardSelection = new wxGridSizer(0, 5, 15, 15);
        SingleCardPanel *CardPanel;
        for ( auto card : selectedCards ) {
            CardPanel = new SingleCardPanel(this, card.first, formatting_constants::DEFAULT_PLAYED_CARD_SIZE, 5);
            CardPanel->setBorderColor(*wxWHITE);
            makeSelectable(CardPanel);
            CardSelection->Add(CardPanel, 0, wxALIGN_CENTER);
        }
        return CardSelection;
    }

    void CardSelectionPanel::makeSelectable(SingleCardPanel *card_panel)
    {
        // This display a little text box when hovering over the card
        card_panel->SetToolTip("Select card");
        card_panel->SetCursor(wxCursor(wxCURSOR_HAND));
        card_panel->makeClickable(
                [this, card_panel](wxMouseEvent & /*event*/)
                {
                    this->switchCardSelectionState(card_panel->getCardName());
                    LOG(INFO) << "Card " << card_panel->getCardName()
                              << " clicked, new selection state: " << selectedCards[card_panel->getCardName()];
                    // Change the border color of the card depending of the selection state
                    wxColour new_border_colour = selectedCards[card_panel->getCardName()] ? *wxYELLOW : *wxWHITE;
                    card_panel->setBorderColor(new_border_colour);
                });
    }
} // namespace client
