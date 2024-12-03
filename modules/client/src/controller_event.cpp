
#include <controller_event.h>

namespace client
{
    wxDEFINE_EVENT(wxEVT_CONTROLLER, wxThreadEvent);

    std::ostream &operator<<(std::ostream &os, ControllerEventType type)
    {
        switch ( type ) {
            case ControllerEventType::SHOW_ERROR:
                os << "ShowError";
                break;
            case ControllerEventType::SHOW_LOBBY_SCREEN:
                os << "ShowLobbyScreen";
                break;
            case ControllerEventType::SHOW_MAIN_GAME_SCREEN:
                os << "ShowMainGameScreen";
                break;
            case ControllerEventType::SHOW_VICTORY_SCREEN:
                os << "ShowVictoryScreen";
                break;
            default:
                LOG(ERROR) << "Unknown ControllerEventType: " << static_cast<int>(type);
                os << "UnknownEventType";
                break;
        }
        return os;
    }

    ShowErrorEventData::ShowErrorEventData(const std::string message) : message(message) {}

    ControllerEvent ControllerEvent::showError(const std::string message) {
        ShowErrorEventData data(message);
        return ControllerEvent(ControllerEventType::SHOW_ERROR, data);
    }

    ControllerEvent::ControllerEvent(ControllerEventType type, std::any data) :
        wxThreadEvent(wxEVT_CONTROLLER), _type(type), _data(std::move(data))
    {}

    ControllerEventType ControllerEvent::getType() const { return _type; }

    wxEvent *ControllerEvent::Clone() const { return new ControllerEvent(*this); }
} // namespace client
