#include <panels/lobby_panel.h>

#include <game_controller.h>
#include <uiElements/image_panel.h>

#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>
#include "shared/utils/logger.h"

// NOLINTBEGIN(bugprone-suspicious-enum-usage)
namespace client
{
    LobbyPanel::LobbyPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024)), NamesSizer(new wxGridSizer(2, 2, 0, 0)),
        playerCount(0)
    {
        wxStaticText *Title = new wxStaticText(this, wxID_ANY, "Lobby");
        Title->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        Title->SetForegroundColour(wxColor(0, 0, 0));
        wxBoxSizer *VerticalSizer = new wxBoxSizer(wxVERTICAL);
        VerticalSizer->Add(Title, 0, wxALIGN_CENTER | wxALL, 5);
        wxPanel *Panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(512, 512));
        VerticalSizer->Add(Panel, 1, wxALIGN_CENTER | wxALL, 5);
        wxButton *StartButton = new wxButton(this, wxID_ANY, "Start Game");
        StartButton->Bind(wxEVT_BUTTON, [](const wxCommandEvent &) { GameController::startGame(); });
        VerticalSizer->Add(StartButton, 0, wxALIGN_CENTER | wxALL, 5);

        wxButton *AddPlayerButton = new wxButton(this, wxID_ANY, "Add Player");
        wxString name = "dummy name";
        AddPlayerButton->Bind(wxEVT_BUTTON, [this, name](const wxCommandEvent &) { this->AddPlayer(name); });
        VerticalSizer->Add(AddPlayerButton, 0, wxALIGN_CENTER | wxALL, 5);


        Panel->SetSizer(NamesSizer);


        this->SetSizerAndFit(VerticalSizer);
    }

    void LobbyPanel::refreshPlayers(std::vector<std::string> &players)
    {
        NamesSizer->Clear(true);
        for ( auto player : players ) {
            LOG(DEBUG) << "Adding player " << player;
            this->AddPlayer(player);
        }
    }


    ///
    /// @brief Adds a player visually to the lobby
    /// This is specifically not doing any logic
    /// @pre playerCount < 4
    /// @post playerCount = playerCount + 1
    void LobbyPanel::AddPlayer(wxString name)
    {
        this->playerCount++;
        wxPanel *Player = new wxPanel(NamesSizer->GetContainingWindow(), wxID_ANY, wxDefaultPosition, wxSize(256, 256));
        Player->SetSizer(new wxBoxSizer(wxVERTICAL));

        std::string SpritePath = "assets/Minion" + std::to_string(this->playerCount) + ".png";

        ImagePanel *LogoPanel = new ImagePanel(Player, // parent element
                                               SpritePath, // path to image
                                               wxBITMAP_TYPE_PNG, // format of image
                                               wxDefaultPosition, // position
                                               wxSize(100, 100), // size
                                               0.0 // rotation
        );
        Player->GetSizer()->Add(LogoPanel, 0, wxALL | wxALIGN_CENTER, 5);
        wxStaticText *Player_name = new wxStaticText(Player, wxID_ANY, name);
        Player_name->SetForegroundColour(wxColor(0, 0, 0));
        Player->GetSizer()->Add(Player_name, 0, wxALL | wxALIGN_CENTER, 5);
        NamesSizer->Add(Player, 1, wxALL | wxALIGN_CENTER);
        NamesSizer->Layout();
    }
} // namespace client
// NOLINTEND(bugprone-suspicious-enum-usage)
