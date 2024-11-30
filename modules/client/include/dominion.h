#pragma once

#include <wx/wx.h>
#include "game_controller.h"

namespace client
{

    /**
     * @brief The main application class
     *
     */
    class Dominion : public wxApp
    {
    public:
        bool OnInit() final;
        GameController &getController();

    private:
        GameController *_controller;
    };

} // namespace client

wxDECLARE_APP(client::Dominion);
