
#include <quick_arg_parser.hpp>
#include <server/args.h>
#include <server/network/server_network_manager.h>
#include <shared/utils/exception.h>

namespace server
{
    struct ArgsImpl : MainArguments<ArgsImpl>
    {
        std::string logFile = option("log-file", 'f', "Log file") = "";
        std::string logLevel = option("log-level", 'l', "Log level") = "warn";
        uint16_t port = option("port", 'p', "Port") = DEFAULT_PORT;
    };

    void die(const std::string &message)
    {
        std::cerr << "Error: " << message << std::endl;
        std::exit(1);
    }

    LogLevel parseLogLevel(const std::string &logLevel)
    {
        if ( logLevel == "debug" ) {
            return LogLevel::DEBUG;
        } else if ( logLevel == "info" ) {
            return LogLevel::INFO;
        } else if ( logLevel == "warn" ) {
            return LogLevel::WARN;
        } else if ( logLevel == "error" ) {
            return LogLevel::ERROR;
        } else {
            throw exception::InvalidArgument("Invalid log level: " + logLevel);
        }
    }

    ServerArgs::ServerArgs(int argc, char **argv)
    {
        try {
            ArgsImpl impl{{argc, argv}};
            _logFile = impl.logFile;
            _logLevel = parseLogLevel(impl.logLevel);
            _port = impl.port;
        } catch ( const exception::Cli &e ) {
            die(e.what());
        } catch ( const QuickArgParserInternals::ArgumentError &e ) {
            die(e.what());
        }
    }

    std::string ServerArgs::getLogFile() { return _logFile; }

    LogLevel ServerArgs::getLogLevel() { return _logLevel; }

    uint16_t ServerArgs::getPort() { return _port; }
} // namespace server
