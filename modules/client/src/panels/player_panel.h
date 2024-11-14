#pragma once

#include <shared/game/game_state/player_base.h>

#include <wx/wx.h>

namespace client
{
    class PlayerPanel : public wxPanel
    {
    public:
        PlayerPanel(wxWindow *parent, wxSize size);

        void DrawPlayer(shared::ReducedPlayer& Player) {};

    private:
    };

} // namespace client
