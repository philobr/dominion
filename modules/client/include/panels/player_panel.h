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
        void drawPlayer(const std::unique_ptr<reduced::Player> &Player, bool is_active);

    private:
        void makePlayable(ImagePanel *Image, unsigned int cardIndex);

        wxSize hand_card_size = wxSize(100, 125);

        /**
         * @brief Create the draw pile panel
         * @param draw_pile_size
         * 
         * @return wxPanel*
         */
        wxPanel* createDrawPilePanel(const unsigned int draw_pile_size);

        /**
         * @brief Create the hand panel
         * 
         * @param std::unique_ptr<reduced::Player>& Player
         * @param size_t card_width_borders
         * @param bool is_active
         * 
         * @return wxPanel*
         */
        wxPanel* createHandPanel(const std::unique_ptr<reduced::Player>& Player, const size_t card_width_borders, const bool is_active);

        /**
         * @brief Create the discard pile panel
         * @param  discard_pile_size
         */
        wxPanel* createDiscardPilePanel(const unsigned int discard_pile_size, const std::string &top_discard_card);
    };

} // namespace client
