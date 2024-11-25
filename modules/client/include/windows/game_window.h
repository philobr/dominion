#pragma once

#include <wx/wx.h>

namespace client
{
    /**
     * @brief The main window of the game
     * only displays one panel depending what stage of the game process we're in
     *
     */
    class GameWindow : public wxFrame
    {
    public:
        GameWindow(const wxString &title, const wxPoint &pos, const wxSize &size);

        /**
         * @brief Show a panel in the main window
         *
         * @param panel
         */
        void showPanel(wxPanel *panel);

        /**
         * @brief Set the status bar message
         *
         * @param message
         */
        void setStatus(const std::string &message);

    private:
        wxBoxSizer *_mainLayout;
        wxStatusBar *_statusBar;

        wxPanel *_currentPanel;
    };

} // namespace client
