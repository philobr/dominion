#pragma once

#include <string>
#include <vector>
#include <wx/sizer.h>
#include <wx/wx.h>

#include <shared/game/cards/card_base.h>
#include <uiElements/single_card_panel.h>
#include <uiElements/text_panel.h>

namespace client
{

    /**
     * @brief This class displays the lobby of the game.
     */
    class CardSelectionPanel : public wxPanel
    {
    public:
        /**
         * @brief Constructor for the lobby panel.
         * @param parent The parent window.
         */
        explicit CardSelectionPanel(wxWindow *parent);

        wxScrolledWindow *createCardSelection();

        void switchCardSelectionState(shared::CardBase::id_t card_id)
        {
            selectedCards[card_id] = !selectedCards[card_id];
            if ( selectedCards[card_id] ) {
                selectedCardCount++;
            } else {
                if ( selectedCardCount == 0 ) {
                    LOG(ERROR) << "decrement unsigned int 0 in " << FUNC_NAME;
                    throw std::runtime_error("unreachable code");
                }
                selectedCardCount--;
            }
        }

        void makeAutoSelection();

        void clearSelection();

    private:
        std::unordered_map<shared::CardBase::id_t, bool> selectedCards;
        unsigned int selectedCardCount = 0;
        TextPanel *SelectedCardCountPanel;
        wxButton *StartButton;

        std::vector<SingleCardPanel *> cardPanels;

        void makeSelectable(SingleCardPanel *card_panel);

        void clickOnSelectableCard(SingleCardPanel *card_panel);
    };

} // namespace client
