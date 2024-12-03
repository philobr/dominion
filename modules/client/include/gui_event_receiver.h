
#pragma once

#include <controller_event.h>
#include <gui.h>
#include <wx/wx.h>

namespace client
{
    class GuiEventReceiver : public wxEvtHandler
    {
    public:
        GuiEventReceiver(Gui* gui);
        ~GuiEventReceiver() override = default;

        // TODO: This is only a temporary solution and should be removed asap
        /**
         * @brief Get the Gui object
         */
        Gui& getGui();

        void onControllerEvent(wxThreadEvent &event);
    private:
        std::unique_ptr<Gui> _gui;
    };
} // namespace client
