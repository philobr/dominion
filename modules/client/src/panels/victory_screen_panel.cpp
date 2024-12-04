#include <panels/victory_screen_panel.h>

namespace client
{
    VictoryScreenPanel::VictoryScreenPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), victory_screen_sizer(new wxBoxSizer(wxVERTICAL))
    {}

    void VictoryScreenPanel::drawVictoryScreen(reduced::GameState &game_state)
    {
        DestroyChildren();
        if ( victory_screen_sizer != nullptr ) {
            delete victory_screen_sizer;
        }

        victory_screen_sizer = new wxBoxSizer(wxVERTICAL);

        wxPanel *container = new wxPanel(this, wxID_ANY);
        container->SetMinSize(wxSize(600, -1));

        wxBoxSizer *container_sizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer *title_sizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText *title = new wxStaticText(container, wxID_ANY, "Score", wxDefaultPosition, wxDefaultSize,
                                               wxALIGN_CENTER_HORIZONTAL);
        wxFont title_font = title->GetFont();
        title_font.SetPointSize(24);
        title_font.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(title_font);

        title_sizer->Add(title, 1, wxALL, 20);
        container_sizer->Add(0, 20);
        container_sizer->Add(title_sizer, 0, wxCENTER);

        std::vector<std::pair<std::string, unsigned int>> players;
        if ( game_state.reduced_player != nullptr ) {
            players.push_back({game_state.reduced_player->getId(), game_state.reduced_player->getVictoryPoints()});
        }

        for ( const auto &enemy : game_state.reduced_enemies ) {
            if ( enemy != nullptr ) {
                players.push_back({enemy->getId(), enemy->getVictoryPoints()});
            }
        }

        std::sort(players.begin(), players.end(), [](const auto &a, const auto &b) { return a.second > b.second; });

        wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(0, 2, 10, 20);
        grid_sizer->AddGrowableCol(1);

        for ( const auto &player : players ) {
            wxStaticText *name_text = new wxStaticText(container, wxID_ANY, player.first + ":");

            wxStaticText *score_text = new wxStaticText(container, wxID_ANY, std::to_string(player.second));

            grid_sizer->Add(name_text, wxSizerFlags().Right());
            grid_sizer->Add(score_text, wxSizerFlags().Left());
        }

        container_sizer->Add(grid_sizer, 0, wxCENTER | wxALL, 20);
        container_sizer->Add(0, 20);

        container->SetSizer(container_sizer);
        victory_screen_sizer->Add(container, 1, wxCENTER | wxALL, 10);
        SetSizer(victory_screen_sizer);

        container_sizer->Fit(container);
        victory_screen_sizer->Fit(this);
        Layout();
    }

    void VictoryScreenPanel::drawTestVictoryScreen()
    {
        DestroyChildren();
        if ( victory_screen_sizer != nullptr ) {
            delete victory_screen_sizer;
        }

        victory_screen_sizer = new wxBoxSizer(wxVERTICAL);

        wxPanel *container = new wxPanel(this, wxID_ANY);
        container->SetMinSize(wxSize(1200, -1));

        wxBoxSizer *container_sizer = new wxBoxSizer(wxVERTICAL);

        wxBoxSizer *title_sizer = new wxBoxSizer(wxHORIZONTAL);

        wxStaticText *title = new wxStaticText(container, wxID_ANY, "https://www.youtube.com/watch?v=dQw4w9WgXcQ",
                                               wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
        wxFont title_font = title->GetFont();
        title_font.SetPointSize(24);
        title_font.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(title_font);

        title_sizer->Add(title, 1, wxALL, 20);
        container_sizer->Add(0, 20);
        container_sizer->Add(title_sizer, 0, wxCENTER);

        std::vector<std::pair<std::string, unsigned int>> players = {
                {"E löl", 69}, {"E blöde siech", 42}, {"E glünggi", 9}, {"E sürmel", 6}};

        std::sort(players.begin(), players.end(), [](const auto &a, const auto &b) { return a.second > b.second; });

        wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(0, 2, 10, 20);
        grid_sizer->AddGrowableCol(1);

        for ( const auto &player : players ) {
            wxStaticText *name_text = new wxStaticText(container, wxID_ANY, player.first + ":");

            wxStaticText *score_text = new wxStaticText(container, wxID_ANY, std::to_string(player.second));

            grid_sizer->Add(name_text, wxSizerFlags().Right());
            grid_sizer->Add(score_text, wxSizerFlags().Left());
        }

        container_sizer->Add(grid_sizer, 0, wxCENTER | wxALL, 20);
        container_sizer->Add(0, 20);

        container->SetSizer(container_sizer);
        victory_screen_sizer->Add(container, 1, wxCENTER | wxALL, 10);
        SetSizer(victory_screen_sizer);

        container_sizer->Fit(container);
        victory_screen_sizer->Fit(this);
        Layout();
    }
} // namespace client