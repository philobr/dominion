
#include <server/args.h>
#include <shared/utils/exception.h>
#include <quick_arg_parser.hpp>

namespace server {
    struct ArgsImpl : MainArguments<ArgsImpl> {
        std::string logFile = option("log-file", 'f', "Log file") = "";
        std::string logLevel = option("log-level", 'l', "Log level") = "warn";
    };

    LogLevel parseLogLevel(const std::string &logLevel) {
        if (logLevel == "debug") {
            return LogLevel::DEBUG;
        } else if (logLevel == "info") {
            return LogLevel::INFO;
        } else if (logLevel == "warn") {
            return LogLevel::WARN;
        } else if (logLevel == "error") {
            return LogLevel::ERROR;
        } else {
            throw exception::InvalidArgument("Invalid log level: " + logLevel);
        }
    }

    ServerArgs::ServerArgs(int argc, char **argv) {
        try {
            ArgsImpl impl{{argc, argv}};

            _logFile = impl.logFile;
            _logLevel = parseLogLevel(impl.logLevel);
        } catch (const exception::Cli &e) {
            std::cerr << "Error: Faild to parse args: " << e.what() << std::endl;
            std::exit(1);
        }
    }

    std::string ServerArgs::getLogFile() {
        return _logFile;
    }

    LogLevel ServerArgs::getLogLevel() {
        return _logLevel;
    }
}
