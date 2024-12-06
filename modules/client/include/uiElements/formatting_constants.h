#pragma once
#include <wx/wx.h>

namespace client
{
    namespace formatting_constants
    {
        // Text formatting constants
        const wxFont DEFAULT_FONT = wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        const wxColour ENEMY_BACKGROUND = wxColour(255, 200, 200);
        const wxColour PLAYER_INFO_BACKGROUND = wxColour(200, 220, 240);

        // Panel formatting constants
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(100, 150);
        const wxSize DEFAULT_PLAYED_CARD_SIZE = wxSize(60, 90);
        const wxSize DEFAULT_CARD_SELECTION_SIZE = DEFAULT_PLAYED_CARD_SIZE;
    } // namespace formatting_constants
} // namespace client