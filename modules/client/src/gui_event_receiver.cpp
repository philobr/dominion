
#include <gui_event_receiver.h>
#include <shared/utils/logger.h>

namespace client
{
    GuiEventReceiver::GuiEventReceiver(Gui *gui) : _gui(gui)
    {
        Bind(wxEVT_CONTROLLER, &GuiEventReceiver::onControllerEvent, this);
    }

    Gui &GuiEventReceiver::getGui() { return *_gui; }

    void GuiEventReceiver::onControllerEvent(wxThreadEvent &event)
    {
        auto &controllerEvent = static_cast<client::ControllerEvent &>(event);

        switch ( controllerEvent.getType() ) {
            case ControllerEventType::SHOW_STATUS:
                {
                    ShowStatusEventData data = controllerEvent.getData<ShowStatusEventData>();
                    _gui->showStatus(std::move(data.message));
                    break;
                }
            case ControllerEventType::SHOW_ERROR:
                {
                    ShowErrorEventData data = controllerEvent.getData<ShowErrorEventData>();
                    _gui->showError(std::move(data.title), std::move(data.message));
                    break;
                }
            case ControllerEventType::SHOW_LOBBY_SCREEN:
                {
                    ShowLobbyScreenEventData data = controllerEvent.getData<ShowLobbyScreenEventData>();
                    _gui->showLobbyScreen(std::move(data.players), data.is_game_master);
                    break;
                }
            case ControllerEventType::SHOW_GAME_SCREEN:
                {
                    ShowGameScreenEventData data = controllerEvent.getData<ShowGameScreenEventData>();
                    _gui->showGameScreen(data.game_state);
                    break;
                }
            case ControllerEventType::SHOW_VICTORY_SCREEN:
                {
                    _gui->showVictoryScreen();
                    break;
                }
            case ControllerEventType::SHOW_CARD_SELECTION_SCREEN:
                {
                    _gui->showCardSelectionScreen();
                    break;
                }
            default:
                {
                    LOG(ERROR) << "Unknown controller event type: " << static_cast<int>(controllerEvent.getType());
                    throw exception::UnreachableCode("Unknown controller event type");
                    break;
                }
        }
    }
} // namespace client
