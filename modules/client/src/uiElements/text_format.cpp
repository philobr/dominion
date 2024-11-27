#include <uiElements/text_panel.h>
#include <uiElements/text_format.h>
#include <wx/gtk/stattext.h>


namespace client {
    
    TextPanel::TextPanel(wxWindow* parent, wxWindowID id, wxString text, TextFormat format) : wxStaticText(parent, id, text) {

      switch (format) {
        case TextFormat::Plain:
              SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
              SetForegroundColour(wxColor(0, 0, 0));
              break;
        case TextFormat::Bold:
              SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
              SetForegroundColour(wxColor(0, 0, 0));
              break;
        case TextFormat::Italic:
              SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_ITALIC, wxFONTWEIGHT_NORMAL));
              SetForegroundColour(wxColor(0, 0, 0));
              break;
        case TextFormat::Title:
              SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
              SetForegroundColour(wxColor(0, 0, 0));
              break;
      }
    }
}
