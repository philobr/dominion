#pragma once

#include <shared/game/game_state/player_base.h>

#include <wx/wx.h>

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
        void DrawPlayer(const std::unique_ptr<shared::ReducedPlayer> &Player);

    private:
    };

} // namespace client
