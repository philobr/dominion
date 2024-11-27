#pragma once

#include <uiElements/text_format.h>
#include <wx/wx.h>


namespace client
{

    class TextPanel : public wxStaticText
    {
    public:
        TextPanel(wxWindow *parent, wxWindowID id, wxString text = "", TextFormat format = TextFormat::Plain);
    };
} // namespace client
