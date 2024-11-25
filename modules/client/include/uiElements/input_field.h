#pragma once

#include <wx/wx.h>

namespace client
{

    class InputField : public wxPanel
    {

    public:
        InputField(wxWindow *parent, const wxString &labelText, int labelWidth, const wxString &fieldValue,
                   int fieldWidth);
        wxString getValue();

    private:
        wxStaticText *_label;
        wxTextCtrl *_field;
    };


} // namespace client
