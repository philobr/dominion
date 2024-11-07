#pragma once

#include <wx/wx.h>

namespace client
{
    class LobbyPanel : public wxPanel
    {
    public:
        explicit LobbyPanel(wxWindow *parent);

        void AddPlayer(wxString name);

    private:
        wxBoxSizer * VerticalSizer;

        unsigned int playerCount;
    };

} // namespace client
