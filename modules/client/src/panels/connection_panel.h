#pragma once

#include <wx/wx.h>
#include "../uiElements/input_field.h"


namespace client
{
    class ConnectionPanel : public wxPanel
    {

    public:
        ConnectionPanel(wxWindow *parent);

        wxString getServerAddress();
        wxString getServerPort();
        wxString getPlayerName();

    private:
        InputField *_serverAddressField;
        InputField *_serverPortField;
        InputField *_playerNameField;
    };

} // namespace client
