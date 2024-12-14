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
        const wxColour DEFAULT_WINDOW_BACKGROUND = wxColour(152, 178, 89); // Light meadow green from the hills
        const wxColour ENEMY_BACKGROUND = wxColour(184, 98, 42); // Warm brown from the dirt path
        const wxColour ACTIVE_ENEMY_BACKGROUND = wxColour(227, 162, 66); // Golden sunset color
        const wxColour ACTIVE_PLAYER_INFO_BACKGROUND = wxColour(95, 143, 87); // Darker grass green
        const wxColour PASSIVE_PLAYER_INFO_BACKGROUND = wxColour(116, 169, 191); // River blue
        const wxColour ACTIVE_PLAYER_PANEL = wxColour(241, 196, 83); // Bright sunset gold
        const wxColour INACTIVE_PLAYER_PANEL = wxColour(137, 157, 143); // Muted green-gray from shadows
        const wxColour ATTACKED_PLAYER_PANEL = wxColour(168, 71, 50); // Dark orange-brown from rooftops


        const wxColour SELECTED_CARD_BACKGROUND = wxColour(255, 203, 96); // Warm golden glow from the sunset
        ;

        // Panel formatting constants
#ifdef __APPLE__
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(100, 150);
#else
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(90, 135);
#endif
        const wxSize DEFAULT_HAND_CARD_SIZE = wxSize(100, 125);
        const wxSize DEFAULT_PLAYED_CARD_SIZE = wxSize(60, 90);
        const wxSize DEFAULT_CARD_SELECTION_SIZE = wxSize(160, 240);

        // Border colours
        const wxColour PLAYABLE_HAND_CARD_BORDER = wxColour(0, 130, 0);

        // Window background iamge
        const wxString WINDOW_BACKGROUND_PATH = "assets/Background.png";
        const wxColour DEFAULT_PANEL_BACKGROUND = wxColour(255, 200, 100); // light orange
        const wxColour EMPTY_PANEL_BACKGROUND = wxColour(200, 200, 200); // light gray

        // Victory screen formatting constants
        const wxSize VICTORY_SCREEN_MIN_SIZE = wxSize(1200, -1);
        const wxSize VICTORY_SCREEN_IMAGE_SIZE = wxSize(500, 500);
        const wxSize VICTORY_SCREEN_TEXT_SIZE = wxSize(100, -1);

        const unsigned int MAX_ENEMY_HAND_CARDS = 7;
    } // namespace formatting_constants
} // namespace client
