#pragma once

#include "image_panel.h"

#include <shared/game/game_state/reduced_game_state.h>

#include <wx/wx.h>


namespace client
{
    /**
     * @brief A utility panel that displays a pile of cards
     */
    class SingleCardPanel : public wxPanel
    {
    public:
        SingleCardPanel(wxWindow *parent, std::string card_name, wxSize size = wxDefaultSize, int padding = 0);

    private:
        ImagePanel *Image;
    };
} // namespace client
