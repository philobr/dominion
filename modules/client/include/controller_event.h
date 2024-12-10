
#pragma once

#include <any>
#include <memory>
#include <shared/action_order.h>
#include <shared/game/game_state/player_base.h>
#include <shared/game/game_state/reduced_game_state.h>
#include <shared/game/reduced/player.h>
#include <shared/player_result.h>
#include <shared/utils/logger.h>
#include <string>
#include <vector>
#include <wx/event.h>

namespace client
{
    wxDECLARE_EVENT(wxEVT_CONTROLLER, wxThreadEvent);

    enum class ControllerEventType
    {
        SHOW_STATUS,
        SHOW_ERROR,
        SHOW_LOBBY_SCREEN,
        SHOW_GAME_SCREEN,
        SHOW_VICTORY_SCREEN,
        SHOW_CARD_SELECTION_SCREEN,
        SHOW_GAIN_FROM_BOARD_SCREEN
    };

    std::ostream &operator<<(std::ostream &os, ControllerEventType type);

    struct ShowStatusEventData
    {
    public:
        ShowStatusEventData(std::string message);

        std::string message;
    };

    struct ShowErrorEventData
    {
    public:
        ShowErrorEventData(std::string title, std::string message);

        std::string title;
        std::string message;
    };

    struct ShowLobbyScreenEventData
    {
    public:
        ShowLobbyScreenEventData(std::vector<std::string> players, bool is_game_master);

        std::vector<std::string> players;
        bool is_game_master;
    };

    struct ShowGameScreenEventData
    {
    public:
        ShowGameScreenEventData(std::unique_ptr<reduced::GameState> game_state);

        // This is hacky bullshit, but this is the easiest way to make this
        // struct copyable. We should really be using a unique_ptr here.
        std::shared_ptr<reduced::GameState> game_state;
    };

    struct ShowGainFromBoardScreenEventData : ShowGameScreenEventData
    {
    public:
        ShowGainFromBoardScreenEventData(std::unique_ptr<reduced::GameState> game_state,
                                         shared::GainFromBoardOrder order);

        shared::GainFromBoardOrder order;
    };

    class ControllerEvent : public wxThreadEvent
    {
    public:
        static ControllerEvent *showStatus(const std::string message);
        static ControllerEvent *showError(const std::string title, const std::string message);
        static ControllerEvent *showLobbyScreen(const std::vector<std::string> players, const bool is_game_master);
        static ControllerEvent *showGameScreen(std::unique_ptr<reduced::GameState> game_state);
        static ControllerEvent *showVictoryScreen(std::vector<shared::PlayerResult> results);
        static ControllerEvent *showCardSelectionScreen();
        static ControllerEvent *showGainFromBoardScreen(std::unique_ptr<reduced::GameState> game_state,
                                                        shared::GainFromBoardOrder order);

        ControllerEvent(ControllerEventType type, std::any data = {});

        ControllerEventType getType() const;

        template <typename T>
        T getData() const
        {
            if ( !_data.has_value() ) {
                LOG(ERROR) << "ControllerEvent: Event of type " << _type << " does not have data";
                throw exception::UnreachableCode("ControllerEvent: This event does not have data");
            }

            try {
                return std::any_cast<T>(_data);
            } catch ( const std::bad_any_cast &e ) {
                LOG(ERROR) << "ControllerEvent: Event of type " << _type << " has data of the wrong type (expected "
                           << typeid(T).name() << ", got " << _data.type().name() << ")";
                throw exception::UnreachableCode("ControllerEvent: Data is not of the expected type");
            }
        }

        // Apparently this is required by wxWidgets
        wxEvent *Clone() const override;

    private:
        ControllerEventType _type;
        std::any _data;
    };
} // namespace client
