#include <uiElements/input_field.h>
#include "uiElements/text_panel.h"

namespace client
{

    InputField::InputField(wxWindow *parent, const wxString &labelText, int labelWidth, const wxString &fieldValue,
                           int fieldWidth) :
        wxPanel(parent, wxID_ANY)
    {

        wxBoxSizer *horizontalLayout = new wxBoxSizer(wxHORIZONTAL);

        this->_label = new TextPanel(this, // parent element
                                     wxID_ANY, // element id
                                     labelText, // text that's displayed as label
                                     TextFormat::PLAIN // text format
        );
        this->_label->SetMinSize(wxSize(labelWidth, -1));
        horizontalLayout->Add(this->_label, 0, wxALIGN_CENTER);

        this->_field = new wxTextCtrl(this, // parent element
                                      wxID_ANY, // element id
                                      fieldValue, // default value
                                      wxDefaultPosition, // position
                                      wxSize(fieldWidth, -1) // size (-1 means default size)
        );
        horizontalLayout->Add(this->_field, 0, wxALIGN_CENTER);
        this->_label->SetForegroundColour(wxColor(*wxBLACK));
        this->_field->SetForegroundColour(wxColor(*wxBLACK));
        this->SetSizerAndFit(horizontalLayout);
    }


    wxString InputField::getValue() { return this->_field->GetValue(); }

} // namespace client
