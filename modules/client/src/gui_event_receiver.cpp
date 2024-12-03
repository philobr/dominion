
#include <gui_event_receiver.h>
#include <shared/utils/logger.h>

namespace client
{
    GuiEventReceiver::GuiEventReceiver(Gui *gui) : _gui(gui)
    {
        Bind(wxEVT_CONTROLLER, &GuiEventReceiver::onControllerEvent, this);
    }

    Gui &GuiEventReceiver::getGui() { return *_gui; }

    void GuiEventReceiver::onControllerEvent(wxThreadEvent & /*event*/)
    {
        LOG(DEBUG) << "GUI received event from GameController";
        LOG(WARN) << "OnControllerEvent handler not implemented";
        // TODO: Implement
    }
} // namespace client
