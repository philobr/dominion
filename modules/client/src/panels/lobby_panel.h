#pragma once

#include <wx/sizer.h>
#include <wx/wx.h>

namespace client
{
    class LobbyPanel : public wxPanel
    {
    public:
        explicit LobbyPanel(wxWindow *parent);

        void AddPlayer(wxString name);

    private:
        wxGridSizer *NamesSizer;

        unsigned int playerCount;
    };

} // namespace client
