#pragma once

#include <wx/wx.h>
#include "../uiElements/input_field.h"


namespace client
{
    /**
     * @brief The panel that allows the user to input the connection details
     *
     */
    class ConnectionPanel : public wxPanel
    {

    public:
        explicit ConnectionPanel(wxWindow *parent);

        /**
         * @brief Get the server address from the input field
         *
         * @return wxString
         */
        wxString getServerAddress();

        /**
         * @brief Get the server port from the input field
         *
         * @return wxString
         */
        wxString getServerPort();

        /**
         * @brief Get the player name from the input field
         *
         * @return wxString
         */
        wxString getPlayerName();

    private:
        InputField *_serverAddressField;
        InputField *_serverPortField;
        InputField *_playerNameField;
    };

} // namespace client
