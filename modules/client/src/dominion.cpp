#include "dominion.h"

#include <shared/utils/logger.h>
#include "game_controller.h"
#include "windows/game_window.h"


namespace client
{

    // Application entry point
    bool Dominion::OnInit()
    {
        // initialize logger
        shared::Logger::initialize();
        shared::Logger::writeTo("logs/client.log");
        shared::Logger::setLevel(INFO);
        LOG(INFO) << "Called Dominion::OnInit()";
        shared::Logger::setLevel(INFO);
        // Allow loading of JPEG  and PNG image files
        wxImage::AddHandler(new wxJPEGHandler());
        wxImage::AddHandler(new wxPNGHandler());

        // Open main game window
        GameWindow *gameWindow = new GameWindow("DOMINION", // title of window,
                                                wxDefaultPosition, // position of the window
                                                wxDefaultSize // size of the window
        );
        gameWindow->Show(true);

        // Initialize game controller
        GameController::init(gameWindow);

        LOG(INFO) << "Done with Dominion::OnInit()";
        return true;
    }

} // namespace client
