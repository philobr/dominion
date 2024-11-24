
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
        uint16_t getPort();

    private:
        std::string _logFile;
        LogLevel _logLevel;
        uint16_t _port;
    };
} // namespace server
