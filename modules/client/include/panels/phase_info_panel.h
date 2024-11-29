#pragma once

#include <shared/game/reduced/player.h>

#include <vector>
#include <wx/wx.h>

namespace client
{
    /**
     * @brief The panel that displays the information about the player 
     * available actions buys etc, also includes the end turn button
     */
    class PhaseInfoPanel : public wxPanel
    {
    public:
        PhaseInfoPanel(wxWindow* parent, wxSize size);

        void drawInfoPanel(const std::unique_ptr<reduced::Player>& player);

        TextPanel *drawPlayerInfo(const std::unique_ptr<reduced::Player>& player);
    private:
    };

} // namespace client