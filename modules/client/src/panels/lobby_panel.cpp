#include "lobby_panel.h"

#include "../game_controller.h"

#include <wx/event.h>
#include <wx/sizer.h>


namespace client
{
    LobbyPanel::LobbyPanel(wxWindow *parent)
        : wxPanel(parent, wxID_ANY), VerticalSizer(new wxBoxSizer(wxVERTICAL))
    {
      wxStaticText* Title = new wxStaticText(this, wxID_ANY, "Lobby");
      VerticalSizer->Add(Title, 0, wxALIGN_CENTER | wxALL, 5);
      wxBoxSizer * HorizontalSizer = new wxBoxSizer(wxHORIZONTAL);
      VerticalSizer->Add(HorizontalSizer, 0, wxALIGN_CENTER | wxALL, 5);
      wxButton* StartButton = new wxButton(this, wxID_ANY, "Start Game");
      StartButton->Bind(wxEVT_BUTTON, [](const wxCommandEvent &event){ GameController::startGame(); });
      VerticalSizer->Add(StartButton, 0, wxALIGN_CENTER | wxALL, 5);

      wxButton* AddPlayerButton = new wxButton(this, wxID_ANY, "Add Player");
      wxString name = "dummy name";
      StartButton->Bind(wxEVT_BUTTON, [this, name](const wxCommandEvent &event){ this->AddPlayer(name); });
      VerticalSizer->Add(AddPlayerButton, 0, wxALIGN_CENTER | wxALL, 5);


      

      this->SetSizerAndFit(VerticalSizer);
    }


    void LobbyPanel::AddPlayer(wxString name)
    {
      wxStaticText* Player = new wxStaticText(this, wxID_ANY, name);
      VerticalSizer->Add(Player, 0, wxALIGN_CENTER | wxALL, 5);
      this->playerCount++;
    }
} // namespace client
