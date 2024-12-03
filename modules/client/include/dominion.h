#pragma once

#include <game_controller.h>
#include <wx/wx.h>

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
        std::unique_ptr<GameController> _controller;
    };

} // namespace client

wxDECLARE_APP(client::Dominion);
