
#include <shared/utils/logger.h>

namespace server
{
    class ServerArgs
    {
    public:
        ServerArgs(int argc, char *argv[]);
        ~ServerArgs() = default;
        std::string getLogFile();
        LogLevel getLogLevel();

    private:
        std::string _logFile;
        LogLevel _logLevel;
    };
} // namespace server
