#pragma once

#include <wx/wx.h>
#include "shared/game_state.h"

namespace client
{
    class BoardPanel : public wxPanel
    {
    public:
        BoardPanel(wxWindow *parent);

    private:
        std::vector<shared::Pile> victory_cards;
        std::vector<shared::Pile> treasure_cards;
        std::vector<shared::Pile> kingdom_cards;
    };

} // namespace client
