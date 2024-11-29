#include <panels/victory_screen_panel.h>

namespace client
{
    VictoryScreenPanel::VictoryScreenPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), victory_screen_sizer(new wxBoxSizer(wxVERTICAL))
    {}

    void VictoryScreenPanel::drawVictoryScreen(reduced::GameState& game_state)
    {
        // Clear the sizer
        this->victory_screen_sizer->Clear(true);

        // Add the title
        wxStaticText* title =
            new wxStaticText(this, wxID_ANY, "Score", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
        title->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        // Use wxCENTER instead of wxALIGN_CENTER
        this->victory_screen_sizer->Add(title, 0, wxCENTER | wxALL, 10);

        // Create a vector of pairs with the name of each player and his points
        std::vector<std::pair<std::string, unsigned int>> players;
        players.push_back(
                std::make_pair(game_state.reduced_player->getId(), game_state.reduced_player->getVictoryPoints()));
        for ( auto &enemy : game_state.reduced_enemies ) {
            players.push_back(std::make_pair(enemy->getId(), enemy->getVictoryPoints()));
        }

        // Sort the players by their points
        std::sort(players.begin(), players.end(),
                  [](const std::pair<std::string, unsigned int> &a, const std::pair<std::string, unsigned int> &b)
                  { return a.second > b.second; });

        // Add the players
        for (auto& player : players) {
            wxStaticText* player_text =
                new wxStaticText(this, wxID_ANY, player.first + ": " + std::to_string(player.second),
                    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
            // Use wxCENTER instead of wxALIGN_CENTER
            this->victory_screen_sizer->Add(player_text, 0, wxCENTER | wxALL, 5);
        }

        // Set minimum size for the panel
        SetMinSize(wxSize(200, -1));

        this->SetSizer(this->victory_screen_sizer);
        this->Layout();
    }

    void VictoryScreenPanel::drawTestVictoryScreen()
    {
        // Clear the sizer
        this->victory_screen_sizer->Clear(true);

        wxStaticText* title =
            new wxStaticText(this, wxID_ANY, "https://www.youtube.com/watch?v=dQw4w9WgXcQ", wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
        title->SetFont(wxFont(24, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        // Remove wxEXPAND, keep only alignment and border
        this->victory_screen_sizer->Add(title, 0, wxCENTER | wxALL, 10);

        // Create a vector of pairs with the name of each player and his points
        std::vector<std::pair<std::string, unsigned int>> players;

        // Create some players and points
        players.push_back(std::make_pair("E löl", 69));
        players.push_back(std::make_pair("E blöde siech", 42));
        players.push_back(std::make_pair("E glünggi", 9));
        players.push_back(std::make_pair("E sürmel", 6));


        // Sort the players by their points
        std::sort(players.begin(), players.end(),
                  [](const std::pair<std::string, unsigned int> &a, const std::pair<std::string, unsigned int> &b)
                  { return a.second > b.second; });

        // Add the players
        for (auto& player : players) {
            wxStaticText* player_text =
                new wxStaticText(this, wxID_ANY, player.first + ": " + std::to_string(player.second),
                    wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
            // Use wxCENTER instead of wxALIGN_CENTER
            this->victory_screen_sizer->Add(player_text, 0, wxCENTER | wxALL, 5);
        }

        // Set minimum size for the panel
        SetMinSize(wxSize(200, -1));

        this->SetSizer(this->victory_screen_sizer);
        this->Layout();
    }
} // namespace client