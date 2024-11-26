#pragma once

#include <shared/game/reduced/player.h>

#include <wx/wx.h>

#include "../uiElements/image_panel.h"

namespace client
{
    /**
     * @brief The panel that displays the information about the player
     * This includes their hand cards, deck size, discard pile size and
     * the number of actions, buys and treasure they have left.
     */
    class PlayerPanel : public wxPanel
    {
    public:
        PlayerPanel(wxWindow *parent, wxSize size);

        /**
         * @brief Draw the player
         *
         * @param Player
         */
        void DrawPlayer(const std::unique_ptr<reduced::Player> &Player, bool is_active);

    private:
        void makePlayable(ImagePanel *Image, std::string card_id);

        wxSize hand_card_size = wxSize(100, 125);
    };

} // namespace client
