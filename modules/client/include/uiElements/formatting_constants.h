#pragma once
#include <wx/gdicmn.h>
#include <wx/wx.h>

namespace client
{
    namespace formatting_constants
    {
        // Text formatting constants
        const wxFont DEFAULT_FONT = wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        // Panel background colours
        const wxColour DEFAULT_WINDOW_BACKGROUND = wxColor(213, 231, 239); // light blue
        const wxColour ENEMY_BACKGROUND = wxColour(255, 200, 200); // light red
        const wxColour ACTIVE_ENEMY_BACKGROUND = wxColour(255, 200, 150); // light orange
        const wxColour ACTIVE_PLAYER_INFO_BACKGROUND = wxColour(95, 100, 55); // light green
        const wxColour PASSIVE_PLAYER_INFO_BACKGROUND = ACTIVE_ENEMY_BACKGROUND; // wxColour(200, 220, 240);
        const wxColour SELECTED_CARD_BACKGROUND = *wxYELLOW;

// Panel formatting constants
#ifdef __APPLE__
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(100, 150);
#else
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(90, 135);
#endif
        const wxSize DEFAULT_PLAYED_CARD_SIZE = wxSize(60, 90);
        const wxSize DEFAULT_CARD_SELECTION_SIZE = wxSize(160, 240);

        // Border colours
        const wxColour PLAYABLE_HAND_CARD_BORDER = wxColour(0, 130, 0);

        // Window background iamge
        const wxString WINDOW_BACKGROUND_PATH = "assets/Background.jpg";
        static bool background_image;
        const wxColour DEFAULT_PANEL_BACKGROUND = wxColour(255, 200, 100); // light orange
    } // namespace formatting_constants
} // namespace client
