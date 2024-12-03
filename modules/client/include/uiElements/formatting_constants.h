#include <wx/wx.h>

namespace client
{
    namespace formatting_constants
    {
        // Text formatting constants
        const wxFont DEFAULT_FONT = wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        const wxColour ENEMY_BACKGROUND = wxColour(255, 200, 200);

        // Panel formatting constants
        const wxSize DEFAULT_BOARD_PILE_SIZE = wxSize(100, 150);
        const wxSize DEFAULT_PLAYED_CARD_SIZE = wxSize(60, 90);
    } // namespace formatting_constants
} // namespace client