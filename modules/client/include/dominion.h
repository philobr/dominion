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
        bool isDebugMode() const;

    private:
        std::unique_ptr<GameController> _controller;
        bool _debug_mode = false;
    };

} // namespace client

wxDECLARE_APP(client::Dominion);
