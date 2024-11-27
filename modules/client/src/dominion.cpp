#include "dominion.h"

#include <shared/utils/logger.h>
#include <wx/cmdline.h>
#include "game_controller.h"
#include "windows/game_window.h"

static const wxCmdLineEntryDesc CMD_LINE_DESC[] = {
        {wxCMD_LINE_SWITCH, "h", "help", "show this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
        {wxCMD_LINE_OPTION, "f", "log-file", "Log file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
        {wxCMD_LINE_OPTION, "l", "log-level", "Log level", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
        {wxCMD_LINE_NONE, nullptr, nullptr, nullptr, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL}};


namespace client
{

    // Application entry point
    bool Dominion::OnInit()
    {
        // Parse command line arguments
        wxCmdLineParser parser(CMD_LINE_DESC, argc, argv);

        if ( parser.Parse() != 0 ) {
            // Error parsing command line arguments
            return false;
        }

        // initialize logger
        shared::Logger::initialize();

        wxString logFile;
        if ( parser.Found("log-file", &logFile) ) {
            shared::Logger::writeTo(logFile.ToStdString());
        }

        wxString logLevel;
        if ( parser.Found("log-level", &logLevel) ) {
            std::optional<LogLevel> parsedLogLevel = shared::parseLogLevel(logLevel.ToStdString());
            if ( parsedLogLevel.has_value() ) {
                shared::Logger::setLevel(parsedLogLevel.value());
            } else {
                std::cerr << "Invalid log level: " << logLevel << std::endl;
                return false;
            }
        }

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
