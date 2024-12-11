
#include <controller_event.h>
#include <shared/player_result.h>

namespace client
{
    wxDEFINE_EVENT(wxEVT_CONTROLLER, wxThreadEvent);

    std::ostream &operator<<(std::ostream &os, ControllerEventType type)
    {
        switch ( type ) {
            case ControllerEventType::SHOW_STATUS:
                os << "ShowStatus";
                break;
            case ControllerEventType::SHOW_ERROR:
                os << "ShowError";
                break;
            case ControllerEventType::SHOW_LOBBY_SCREEN:
                os << "ShowLobbyScreen";
                break;
            case ControllerEventType::SHOW_GAME_SCREEN:
                os << "ShowMainGameScreen";
                break;
            case ControllerEventType::SHOW_VICTORY_SCREEN:
                os << "ShowVictoryScreen";
                break;
            case ControllerEventType::SHOW_CARD_SELECTION_SCREEN:
                os << "ShowCardSelectionScreen";
                break;
            case ControllerEventType::SHOW_GAIN_FROM_BOARD_SCREEN:
                os << "ShowGainFromBoardScreen";
                break;
            case ControllerEventType::SHOW_CHOOSE_FROM_HAND_SCREEN:
                os << "ShowChooseFromHandScreen";
                break;
            default:
                LOG(ERROR) << "Unknown ControllerEventType: " << static_cast<int>(type);
                os << "UnknownEventType";
                break;
        }
        return os;
    }

    ShowStatusEventData::ShowStatusEventData(std::string message) : message(std::move(message)) {}

    ShowErrorEventData::ShowErrorEventData(std::string title, std::string message) :
        title(std::move(title)), message(std::move(message))
    {}

    ShowLobbyScreenEventData::ShowLobbyScreenEventData(std::vector<reduced::Player::id_t> players,
                                                       bool is_game_master) :
        players(std::move(players)),
        is_game_master(is_game_master)
    {}

    ShowGameScreenEventData::ShowGameScreenEventData(std::unique_ptr<reduced::GameState> game_state) :
        game_state(std::move(game_state))
    {}

    ShowGainFromBoardScreenEventData::ShowGainFromBoardScreenEventData(std::unique_ptr<reduced::GameState> game_state,
                                                                       shared::GainFromBoardOrder order) :
        ShowGameScreenEventData(std::move(game_state)),
        order(order)
    {}

    ShowChooseFromHandScreenEventData::ShowChooseFromHandScreenEventData(std::unique_ptr<reduced::GameState> game_state,
                                                                         shared::ChooseFromHandOrder order) :
        ShowGameScreenEventData(std::move(game_state)),
        order(order)
    {}

    ControllerEvent *ControllerEvent::showStatus(std::string status)
    {
        ShowStatusEventData data(std::move(status));
        return new ControllerEvent(ControllerEventType::SHOW_STATUS, data);
    }

    ControllerEvent *ControllerEvent::showError(std::string title, std::string message)
    {
        ShowErrorEventData data(std::move(title), std::move(message));
        return new ControllerEvent(ControllerEventType::SHOW_ERROR, data);
    }

    ControllerEvent *ControllerEvent::showLobbyScreen(std::vector<reduced::Player::id_t> players, bool is_game_master)
    {
        ShowLobbyScreenEventData data(std::move(players), is_game_master);
        return new ControllerEvent(ControllerEventType::SHOW_LOBBY_SCREEN, data);
    }

    ControllerEvent *ControllerEvent::showGameScreen(std::unique_ptr<reduced::GameState> game_state)
    {
        ShowGameScreenEventData data(std::move(game_state));
        return new ControllerEvent(ControllerEventType::SHOW_GAME_SCREEN, data);
    }

    ControllerEvent *ControllerEvent::showVictoryScreen(std::vector<shared::PlayerResult> results)
    {
        return new ControllerEvent(ControllerEventType::SHOW_VICTORY_SCREEN, std::move(results));
    }

    ControllerEvent *ControllerEvent::showCardSelectionScreen()
    {
        return new ControllerEvent(ControllerEventType::SHOW_CARD_SELECTION_SCREEN, std::any());
    }

    ControllerEvent *ControllerEvent::showGainFromBoardScreen(std::unique_ptr<reduced::GameState> game_state,
                                                              shared::GainFromBoardOrder order)
    {
        ShowGainFromBoardScreenEventData data(std::move(game_state), order);
        return new ControllerEvent(ControllerEventType::SHOW_GAIN_FROM_BOARD_SCREEN, data);
    }

    ControllerEvent *ControllerEvent::showChooseFromHandScreen(std::unique_ptr<reduced::GameState> game_state,
                                                               shared::ChooseFromHandOrder order)
    {
        ShowChooseFromHandScreenEventData data(std::move(game_state), order);
        return new ControllerEvent(ControllerEventType::SHOW_CHOOSE_FROM_HAND_SCREEN, data);
    }

    ControllerEvent::ControllerEvent(ControllerEventType type, std::any data) :
        wxThreadEvent(wxEVT_CONTROLLER), _type(type), _data(std::move(data))
    {}

    ControllerEventType ControllerEvent::getType() const { return _type; }

    wxEvent *ControllerEvent::Clone() const { return new ControllerEvent(*this); }
} // namespace client
