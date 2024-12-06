#pragma once

#include <string>
#include <vector>
#include <wx/sizer.h>
#include <wx/wx.h>

#include <shared/game/cards/card_base.h>
#include <uiElements/single_card_panel.h>

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
        }

    private:
        std::unordered_map<shared::CardBase::id_t, bool> selectedCards;

        void makeSelectable(SingleCardPanel *card_panel);
    };

} // namespace client
