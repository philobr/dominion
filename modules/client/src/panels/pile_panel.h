#pragma once

#include "../uiElements/image_panel.h"

#include <shared/game/game_state/reduced_game_state.h>

#include <wx/wx.h>


namespace client
{
    /**
     * @brief A utility panel that displays a pile of cards
     */
    class PilePanel : public wxPanel
    {
    public:
        PilePanel(wxWindow *parent, shared::Pile pile);
    };
} // namespace client
