#include <panels/victory_screen_panel.h>
#include <uiElements/text_panel.h>

namespace client
{
    VictoryScreenPanel::VictoryScreenPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), victory_screen_sizer(new wxBoxSizer(wxVERTICAL))
    {}

    void VictoryScreenPanel::drawVictoryScreen(const std::vector<shared::PlayerResult> &player_results)
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

        TextPanel *title = new TextPanel(container, wxID_ANY, "Score", TextFormat::TITLE);

        title_sizer->Add(title, 1, wxALL, 20);
        container_sizer->Add(0, 20);
        container_sizer->Add(title_sizer, 0, wxCENTER);

        wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(0, 2, 10, 20);
        grid_sizer->AddGrowableCol(1);

        for ( const auto &player : player_results ) {
            TextPanel *name_text = new TextPanel(container, wxID_ANY, player.playerName() + ":", TextFormat::BOLD);

            TextPanel *score_text =
                    new TextPanel(container, wxID_ANY, std::to_string(player.score()), TextFormat::BOLD);

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

        TextPanel *title =
                new TextPanel(container, wxID_ANY, "https://www.youtube.com/watch?v=dQw4w9WgXcQ", TextFormat::TITLE);

        title_sizer->Add(title, 1, wxALL, 20);
        container_sizer->Add(0, 20);
        container_sizer->Add(title_sizer, 0, wxCENTER);

        std::vector<shared::PlayerResult> players = {
                {"E löl", 69}, {"E blöde siech", 42}, {"E glünggi", 9}, {"E sürmel", 6}};

        wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(0, 2, 10, 20);
        grid_sizer->AddGrowableCol(1);

        for ( const auto &player : players ) {
            TextPanel *name_text = new TextPanel(container, wxID_ANY, player.playerName() + ":", TextFormat::BOLD);

            TextPanel *score_text =
                    new TextPanel(container, wxID_ANY, std::to_string(player.score()), TextFormat::BOLD);

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
