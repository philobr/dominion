#include <panels/victory_screen_panel.h>
#include <uiElements/text_panel.h>
#include <uiElements/image_panel.h>

namespace client
{
    VictoryScreenPanel::VictoryScreenPanel(wxWindow *parent) :
        wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize), victory_screen_sizer(new wxBoxSizer(wxVERTICAL))
    {}

    // void VictoryScreenPanel::drawVictoryScreen(const std::vector<shared::PlayerResult> &player_results)
    // {
    //     DestroyChildren();
    //     if ( victory_screen_sizer != nullptr ) {
    //         delete victory_screen_sizer;
    //     }

    //     victory_screen_sizer = new wxBoxSizer(wxVERTICAL);

    //     wxPanel *container = new wxPanel(this, wxID_ANY);
    //     container->SetMinSize(wxSize(600, -1));

    //     wxBoxSizer *container_sizer = new wxBoxSizer(wxVERTICAL);

    //     wxBoxSizer *title_sizer = new wxBoxSizer(wxHORIZONTAL);

    //     TextPanel *title = new TextPanel(container, wxID_ANY, "Score", TextFormat::TITLE);

    //     title_sizer->Add(title, 1, wxALL, 20);
    //     container_sizer->Add(0, 20);
    //     container_sizer->Add(title_sizer, 0, wxCENTER);

    //     wxFlexGridSizer *grid_sizer = new wxFlexGridSizer(0, 2, 10, 20);
    //     grid_sizer->AddGrowableCol(1);

    //     for ( const auto &player : player_results ) {
    //         TextPanel *name_text = new TextPanel(container, wxID_ANY, player.playerName() + ":", TextFormat::BOLD);

    //         TextPanel *score_text =
    //                 new TextPanel(container, wxID_ANY, std::to_string(player.score()), TextFormat::BOLD);

    //         grid_sizer->Add(name_text, wxSizerFlags().Right());
    //         grid_sizer->Add(score_text, wxSizerFlags().Left());
    //     }

    //     container_sizer->Add(grid_sizer, 0, wxCENTER | wxALL, 20);
    //     container_sizer->Add(0, 20);

    //     container->SetSizer(container_sizer);
    //     victory_screen_sizer->Add(container, 1, wxCENTER | wxALL, 10);
    //     SetSizer(victory_screen_sizer);

    //     container_sizer->Fit(container);
    //     victory_screen_sizer->Fit(this);
    //     Layout();
    // }

    void VictoryScreenPanel::drawVictoryScreen(const std::vector<shared::PlayerResult>& player_results)
    {
        DestroyChildren();
        if (victory_screen_sizer != nullptr) {
            delete victory_screen_sizer;
        }

        victory_screen_sizer = new wxBoxSizer(wxVERTICAL);

        // Set min size for victory screen
        SetMinSize(wxSize(900, -1));

        // Show the winner in a special line
        wxBoxSizer* winner_sizer = new wxBoxSizer(wxHORIZONTAL);
        TextPanel* winner_text = new TextPanel(this, wxID_ANY, "Winner:", TextFormat::TITLE);
        TextPanel* winner_score_text = new TextPanel(this, wxID_ANY, player_results[0].playerName(), TextFormat::TITLE);

        winner_sizer->Add(winner_text, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
        winner_sizer->Add(winner_score_text, 0, wxALIGN_CENTER_VERTICAL);

        // Add a spacer for top padding
        victory_screen_sizer->Add(0, 20, 0, wxEXPAND); // 20 pixels tall spacer

        // Add the winner sizer to the main victory screen sizer
        victory_screen_sizer->Add(winner_sizer, 0, wxALIGN_CENTER | wxALL, 10);

        // Create a horizontal sizer for the image and text
        wxBoxSizer* horizontal_sizer = new wxBoxSizer(wxHORIZONTAL);

        // Add the image to the horizontal sizer
        ImagePanel* godminion = new ImagePanel(this, "God_Mode.png", wxBITMAP_TYPE_PNG, wxDefaultPosition,
            wxSize(500, 500), 0.0);
        horizontal_sizer->Add(godminion, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 10);

        // Create a grid sizer for the player results
        wxFlexGridSizer* grid_sizer = new wxFlexGridSizer(0, 2, 10, 20);
        grid_sizer->AddGrowableCol(1);

        for (const auto& player : player_results) {
            TextPanel* name_text = new TextPanel(this, wxID_ANY, player.playerName() + ":", TextFormat::BOLD);
            TextPanel* score_text = new TextPanel(this, wxID_ANY, std::to_string(player.score()), TextFormat::BOLD);

            grid_sizer->Add(name_text, wxSizerFlags().Right());
            grid_sizer->Add(score_text, wxSizerFlags().Left());
        }

        // Add the grid sizer to the horizontal sizer
        horizontal_sizer->Add(grid_sizer, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 20);

        // Add the horizontal sizer to the main vertical sizer
        victory_screen_sizer->Add(horizontal_sizer, 0, wxALIGN_CENTER | wxALL, 20);

        // Create the close button
        wxButton* close_button = new wxButton(this, wxID_ANY, "Close Game");
        victory_screen_sizer->Add(close_button, 0, wxALIGN_CENTER | wxALL, 20);

        // Bind the button click event to a handler
        close_button->Bind(wxEVT_BUTTON, &VictoryScreenPanel::OnCloseButtonClicked, this);

        // skipToVictoryScreenButton->Bind(wxEVT_BUTTON,
        //     [](const wxCommandEvent& /*event*/)
        //     { wxGetApp().getController().skipToVictoryScreen(); });
            
        SetSizerAndFit(victory_screen_sizer);
    }

    void VictoryScreenPanel::OnCloseButtonClicked(wxCommandEvent& /*event*/)
    {
        // Assuming this panel is part of the main frame or you have a way to access it
        wxWindow* mainFrame = wxTheApp->GetTopWindow();
        if (mainFrame != nullptr) {
            mainFrame->Close(true);  // Close the main frame, which should close the application
        }
    }
} // namespace client
