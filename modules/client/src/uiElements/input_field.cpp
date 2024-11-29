#include <uiElements/input_field.h>
#include "uiElements/text_panel.h"

namespace client
{

    InputField::InputField(wxWindow *parent, const wxString &label_text, int label_width, const wxString &field_value,
                           int field_width) :
        wxPanel(parent, wxID_ANY)
    {

        wxBoxSizer *horizontalLayout = new wxBoxSizer(wxHORIZONTAL);

        this->_label = new TextPanel(this, // parent element
                                     wxID_ANY, // element id
                                     label_text, // text that's displayed as label
                                     TextFormat::PLAIN // text format
        );
        this->_label->SetMinSize(wxSize(label_width, -1));
        horizontalLayout->Add(this->_label, 0, wxALIGN_CENTER);

        this->_field = new wxTextCtrl(this, // parent element
                                      wxID_ANY, // element id
                                      field_value, // default value
                                      wxDefaultPosition, // position
                                      wxSize(field_width, -1) // size (-1 means default size)
        );
        horizontalLayout->Add(this->_field, 0, wxALIGN_CENTER);
        this->_label->SetForegroundColour(wxColor(*wxBLACK));
        this->_field->SetForegroundColour(wxColor(*wxBLACK));
        this->SetSizerAndFit(horizontalLayout);
    }


    wxString InputField::getValue() { return this->_field->GetValue(); }

} // namespace client
