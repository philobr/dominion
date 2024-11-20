#pragma once

#include <wx/wx.h>
// #include "../windows/GameWindow.h"
// #include "../GameController.h"

namespace client
{

    /**
     * @brief The main application class
     *
     */
    class Dominion : public wxApp
    {
    public:
        virtual bool OnInit();
    };

} // namespace client
