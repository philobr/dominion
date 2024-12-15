
#pragma once

#include <controller_event.h>
#include <gui.h>
#include <wx/wx.h>

namespace client
{
    /**
     * @brief The single point of entry for all events that are triggered by the controller
     *
     * All interaction with the GUI should be done through this class.
     */
    class GuiEventReceiver : public wxEvtHandler
    {
    public:
        GuiEventReceiver(Gui *gui);
        ~GuiEventReceiver() override = default;

        /**
         * @brief Get the Gui object
         */
        Gui &getGui();

        /**
         * @brief This is called by wxWidgets when a controller event is triggered
         *
         * @param event The event that was triggered by the controller
         */
        void onControllerEvent(wxThreadEvent &event);

    private:
        std::unique_ptr<Gui> _gui;
    };
} // namespace client
