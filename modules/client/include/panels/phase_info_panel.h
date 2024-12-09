#pragma once

#include <shared/game/game_state/reduced_game_state.h>
#include <shared/game/reduced/player.h>
#include <uiElements/formatting_constants.h>
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
        PhaseInfoPanel(wxWindow *parent, wxSize size);

        void drawInfoPanel(const reduced::GameState &game_state);

    private:
        TextPanel *drawPlayerInfo(const shared::PlayerBase &player, const shared::GamePhase &game_phase);

        wxPanel *drawPlayedPanel(std::vector<shared::CardBase::id_t> played_cards);

        wxPanel *drawPlayedPanelTest();

        wxPanel *drawButtonPanel(const reduced::GameState &game_state);

        wxButton *createEndActionButton(wxWindow *parent);
        wxButton *createEndTurnButton(wxWindow *parent);
    };

} // namespace client
