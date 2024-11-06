#pragma once

#include <wx/wx.h>

namespace client
{
    class GameWindow : public wxFrame
    {
    public:
        GameWindow(const wxString &title, const wxPoint &pos, const wxSize &size);

        void showPanel(wxPanel *panel);
        void setStatus(const std::string &message);

    private:
        wxBoxSizer *_mainLayout;
        wxStatusBar *_statusBar;

        wxPanel *_currentPanel;
    };

} // namespace client
