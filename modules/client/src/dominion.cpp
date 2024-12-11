#include "dominion.h"

#include <execinfo.h>
#include <signal.h>

#include <gui_event_receiver.h>
#include <shared/utils/logger.h>
#include <stdexcept>
#include <wx/cmdline.h>
#include "game_controller.h"
#include "uiElements/image_map.h"
#include "windows/game_window.h"

static const wxCmdLineEntryDesc CMD_LINE_DESC[] = {
        {wxCMD_LINE_SWITCH, "h", "help", "show this help message", wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP},
        {wxCMD_LINE_OPTION, "f", "log-file", "Log file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
        {wxCMD_LINE_OPTION, "l", "log-level", "Log level", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
        {wxCMD_LINE_SWITCH, "D", "debug", "Enable debug mode", wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL},
        {wxCMD_LINE_NONE, nullptr, nullptr, nullptr, wxCMD_LINE_VAL_NONE, wxCMD_LINE_PARAM_OPTIONAL}};


void segFaultHandler(int sig)
{
    void *array[10];
    size_t size;

    // get void*'s for all entries on the stack
    size = backtrace(array, 10);

    // print out all the frames to stderr
    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
    exit(1);
}


namespace client
{

    // Application entry point
    bool Dominion::OnInit()
    {
        // Register signal handler for segfaults
        signal(SIGSEGV, segFaultHandler);

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
        } else {
            shared::Logger::setLevel(LogLevel::DEBUG);
        }

        LOG(DEBUG) << "Initialized logger, log level: " << shared::Logger::getLevel();

        if ( parser.Found("debug") ) {
            _debug_mode = true;
        } else {
            _debug_mode = false;
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

        _imageMap = std::make_unique<ImageMap>();
        if ( !_imageMap ) {
            throw std::runtime_error("Failed to create ImageMap");
        }
        Gui *gui = new Gui(gameWindow);
        GuiEventReceiver *eventReceiver = new GuiEventReceiver(gui);
        _controller = std::make_unique<GameController>(eventReceiver);

        return true;
    }

    GameController &Dominion::getController() { return *_controller; }
    ImageMap &Dominion::getImageMap() { return *_imageMap; }

    bool Dominion::isDebugMode() const { return _debug_mode; }

} // namespace client
