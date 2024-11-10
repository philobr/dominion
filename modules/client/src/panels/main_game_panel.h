#pragma once

#include <wx/wx.h>

namespace client
{

    class MainGamePanel : public wxPanel
    {

    public:
        explicit MainGamePanel(wxWindow *parent);


    private:
        // define key constant layout values

        const wxSize VerticalBaseSize = wxSize(1024, 100);
        const wxSize PanelSize = wxSize(1024, 1024);
    };

} // namespace client
