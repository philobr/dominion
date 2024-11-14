#pragma once

#include <shared/game/game_state/board_base.h>

#include <wx/wx.h>

namespace client
{
    class BoardPanel : public wxPanel
    {
    public:
        BoardPanel(wxWindow *parent, wxSize size);

        void DrawBoard(shared::Board &Board){};

    private:
    };

} // namespace client
