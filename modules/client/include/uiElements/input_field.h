#pragma once

#include <wx/wx.h>

namespace client
{

    class InputField : public wxPanel
    {

    public:
        InputField(wxWindow *parent, const wxString &label_text, int label_width, const wxString &field_value,
                   int field_width);
        wxString getValue();

    private:
        wxStaticText *_label;
        wxTextCtrl *_field;
    };


} // namespace client
