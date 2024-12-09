#include <uiElements/text_format.h>
#include <uiElements/text_panel.h>

#ifdef __APPLE__
#include <wx/stattext.h>
#else
#include <wx/gtk/stattext.h>
#endif

namespace client
{

    TextPanel::TextPanel(wxWindow *parent, wxWindowID id, wxString text, TextFormat format) :
        wxStaticText(parent, id, text)
    {

        switch ( format ) {
            case TextFormat::PLAIN:
                SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
                SetForegroundColour(wxColor(0, 0, 0));
                break;
            case TextFormat::BOLD:
                SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
                SetForegroundColour(wxColor(0, 0, 0));
                break;
            case TextFormat::BOLD_SMALL:
            	SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
                SetForegroundColour(wxColor(0, 0, 0));
                break;
            case TextFormat::ITALIC:
                SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
                SetForegroundColour(wxColor(0, 0, 0));
                break;
            case TextFormat::TITLE:
                SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
                SetForegroundColour(wxColor(0, 0, 0));
                break;
        }
    }
} // namespace client
