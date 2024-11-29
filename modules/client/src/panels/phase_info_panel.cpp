#include <panels/phase_info_panel.h>
#include <shared/utils/logger.h>
#include <uiElements/text_panel.h>
#include <wx/wx.h>

namespace client
{
    PhaseInfoPanel::PhaseInfoPanel(wxWindow* parent, wxSize size) : wxPanel(parent, wxID_ANY, wxDefaultPosition, size)
    {
        LOG(WARN) << "using hard coded player";
        auto player = shared::PlayerBase("gigu");
        auto reduced = reduced::Player::make(player, { "Village", "Copper", "Copper", "Copper", "Estate" });

        // Set background color to light blue
        SetBackgroundColour(wxColour(200, 220, 240));

        this->drawInfoPanel(reduced);
    }

    void PhaseInfoPanel::drawInfoPanel(const std::unique_ptr<reduced::Player>& player)
    {
        this->DestroyChildren();

        // Create vertical BoxSizer
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        // Add player info to the sizer
        auto* infoPanel = drawPlayerInfo(player);
        sizer->Add(infoPanel, 0, wxALL, 5);

        //
        // Set minimum width for the info bar
        SetMinSize(wxSize(150, -1));  // 150 pixels wide, height automatic

        this->SetSizer(sizer);
        sizer->Layout();
    }

    TextPanel* PhaseInfoPanel::drawPlayerInfo(const std::unique_ptr<reduced::Player>& player)
    {
        wxString info = wxString::Format(
            "%s\nGold: %d\nActions: %d\nBuys: %d",
            player->getId(),
            player->getTreasure(),
            player->getActions(),
            player->getBuys()
        );

        return new TextPanel(this, wxID_ANY, info);
    }
} // namespace client
