#pragma once

#include <shared/game/reduced/player.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <uiElements/text_panel.h>

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

        void drawInfoPanel(const reduced::GameState& game_state);
    private:
        wxSize played_card_size = wxSize(100, 125);

        TextPanel* drawPlayerInfo(const std::unique_ptr<reduced::Player>& player);

        wxPanel* drawPlayedPanel(std::vector<shared::CardBase::id_t> played_cards);

        wxButton* getEndActionButton();
        wxButton* getEndTurnButton();
    };

} // namespace client