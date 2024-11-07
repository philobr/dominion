#include "lobby_panel.h"

#include "../game_controller.h"

#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>


namespace client
{
    LobbyPanel::LobbyPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024)), NamesSizer(new wxBoxSizer(wxHORIZONTAL))
    {
        wxStaticText *Title = new wxStaticText(this, wxID_ANY, "Lobby");
        wxBoxSizer *VerticalSizer = new wxBoxSizer(wxVERTICAL);
        VerticalSizer->Add(Title, 0, wxALIGN_CENTER | wxALL, 5);
        wxPanel *Panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(1024, 512));
        Panel->SetBackgroundColour(wxColour(0, 100, 0));
        VerticalSizer->Add(Panel, 1, wxALIGN_CENTER | wxALL, 5);
        wxButton *StartButton = new wxButton(this, wxID_ANY, "Start Game");
        StartButton->Bind(wxEVT_BUTTON, [](const wxCommandEvent &event) { GameController::startGame(); });
        VerticalSizer->Add(StartButton, 0, wxALIGN_CENTER | wxALL, 5);

        wxButton *AddPlayerButton = new wxButton(this, wxID_ANY, "Add Player");
        wxString name = "dummy name";
        AddPlayerButton->Bind(wxEVT_BUTTON, [this, name](const wxCommandEvent &event) { this->AddPlayer(name); });
        VerticalSizer->Add(AddPlayerButton, 0, wxALIGN_CENTER | wxALL, 5);


        Panel->SetSizer(NamesSizer);


        this->SetSizerAndFit(VerticalSizer);
    }


    void LobbyPanel::AddPlayer(wxString name)
    {
        wxStaticText *Player = new wxStaticText(NamesSizer->GetContainingWindow(), wxID_ANY, name);
        NamesSizer->Add(Player, 0, wxALL, 5);
        this->playerCount++;
    }
} // namespace client
