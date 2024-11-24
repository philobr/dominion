#include "client/dominion.h"

#include "game_controller.h"
#include "shared/utils/logger.h"
#include "windows/game_window.h"


namespace client
{

    // Application entry point
    bool Dominion::OnInit()
    {
        // Allow loading of JPEG  and PNG image files
        wxImage::AddHandler(new wxJPEGHandler());
        wxImage::AddHandler(new wxPNGHandler());

        // Open main game window
        GameWindow *gameWindow = new GameWindow("LAMA", // title of window,
                                                wxDefaultPosition, // position of the window
                                                wxDefaultSize // size of the window
        );
        gameWindow->Show(true);

        shared::Logger::initialize();
        shared::Logger::setLevel(DEBUG);

        // Initialize game controller
        GameController::init(gameWindow);

        return true;
    }

} // namespace client
