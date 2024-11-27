#include <panels/lobby_panel.h>

#include <game_controller.h>
#include <uiElements/image_panel.h>

#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/sizer.h>
#include <wx/wx.h>
#include "shared/utils/logger.h"
#include "uiElements/text_panel.h"

// NOLINTBEGIN(bugprone-suspicious-enum-usage)
namespace client
{
    LobbyPanel::LobbyPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1024, 1024)), NamesSizer(new wxGridSizer(2, 2, 0, 0)),
        playerCount(0)
    {
        TextPanel *Title = new TextPanel(this, wxID_ANY, "Lobby", TextFormat::Title);
        wxBoxSizer *VerticalSizer = new wxBoxSizer(wxVERTICAL);
        VerticalSizer->Add(Title, 0, wxALIGN_CENTER | wxALL, 5);
        wxPanel *Panel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(512, 512));
        VerticalSizer->Add(Panel, 1, wxALIGN_CENTER | wxALL, 5);
        wxButton *StartButton = new wxButton(this, wxID_ANY, "Start Game");
        StartButton->Bind(wxEVT_BUTTON, [](const wxCommandEvent &) { GameController::startGame(); });
        VerticalSizer->Add(StartButton, 0, wxALIGN_CENTER | wxALL, 5);

        wxButton *AddPlayerButton = new wxButton(this, wxID_ANY, "Add Player");
        wxString name = "dummy name";
        AddPlayerButton->Bind(wxEVT_BUTTON, [this, name](const wxCommandEvent &) { this->addPlayer(name); });
        VerticalSizer->Add(AddPlayerButton, 0, wxALIGN_CENTER | wxALL, 5);


        Panel->SetSizer(NamesSizer);


        this->SetSizerAndFit(VerticalSizer);
    }

    void LobbyPanel::refreshPlayers(std::vector<std::string> &players)
    {
        NamesSizer->Clear(false);
        playerCount = 0;
        for ( auto player : players ) {
            LOG(DEBUG) << "Adding player " << player;
            this->addPlayer(player);
        }
    }


    ///
    /// @brief Adds a player visually to the lobby
    /// This is specifically not doing any logic
    /// @pre playerCount < 4
    /// @post playerCount = playerCount + 1
    void LobbyPanel::addPlayer(wxString name)
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
        TextPanel *Player_name = new TextPanel(Player, wxID_ANY, name, TextFormat::Plain);
        Player->GetSizer()->Add(Player_name, 0, wxALL | wxALIGN_CENTER, 5);
        NamesSizer->Add(Player, 1, wxALL | wxALIGN_CENTER);
        NamesSizer->Layout();
    }
} // namespace client
// NOLINTEND(bugprone-suspicious-enum-usage)
