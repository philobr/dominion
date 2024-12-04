
#include <shared/utils/logger.h>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>

std::ostream &operator<<(std::ostream &os, const LogLevel &level)
{
    os << shared::log_helpers::toString(level);
    return os;
}

namespace shared
{
    std::optional<LogLevel> parseLogLevel(const std::string &log_level)
    {
        if ( log_level == "debug" ) {
            return LogLevel::DEBUG;
        } else if ( log_level == "info" ) {
            return LogLevel::INFO;
        } else if ( log_level == "warn" ) {
            return LogLevel::WARN;
        } else if ( log_level == "error" ) {
            return LogLevel::ERROR;
        } else {
            return std::nullopt;
        }
    }

    // ================================
    // IMPLEMENTATION LogStream
    // ================================
    Logger::LogStream::LogStream(Logger &logger, LogLevel level, const char *file, int line) :
        logger_(logger), level_(level)
    {
        ss_ << log_helpers::formatTimestamp() << " ";
        ss_ << log_helpers::formatLogLevel(level, logger_.log_to_file_) << " ";
        ss_ << log_helpers::formatFileLine(file, line) << " - ";
    }

    Logger::LogStream::~LogStream() { logger_.writeLog(level_, ss_.str()); }

    // ================================
    // IMPLEMENTATION Logger
    // ================================

    Logger::Logger() : min_log_level_(LogLevel::WARN), log_to_file_(false) {}

    Logger::~Logger()
    {
        if ( log_file_.is_open() ) {
            log_file_.flush();
            if ( log_to_file_ ) {
                log_file_ << "[INFO] - END LOG" << std::endl;
            }
            log_file_.close();
        }
    }

    void Logger::initialize()
    {
        std::lock_guard<std::mutex> lock(_init_mutex);
        if ( !_instance ) {
            _instance = std::unique_ptr<Logger>(new Logger());
            LOG(LogLevel::INFO) << "Logger initialized with default settings.";
        } else {
            throw exception::Logger("Logger has already been initialized.");
        }
    }

    Logger &Logger::getInstance()
    {
        if ( !_instance ) {
            initialize(); // default init logs to console
            LOG(LogLevel::WARN) << "Logger not initialized; using default settings.";
        }

        return *_instance;
    }

    void Logger::writeTo(const std::string &file_path)
    {
        std::lock_guard<std::mutex> lock(_init_mutex);
        Logger &logger = getInstance();

        if ( logger.log_file_.is_open() ) {
            logger.log_file_.close();
        }

        if ( !file_path.empty() ) {
            std::filesystem::path log_path = file_path;
            if ( log_path.has_parent_path() ) {
                std::filesystem::create_directories(log_path.parent_path());
            }

            LOG(LogLevel::INFO) << "Logging to file: " << file_path;
            logger.log_file_.open(file_path, std::ios::out | std::ios::app);
            if ( !logger.log_file_ ) {
                throw exception::Logger("Failed to open log file: " + file_path);
            }
            logger.log_to_file_ = true;
        } else {
            logger.log_to_file_ = false;
            LOG(LogLevel::INFO) << "Logging to std::cerr.";
        }
    }

    void Logger::setLevel(LogLevel level)
    {
        std::lock_guard<std::mutex> lock(_init_mutex);
        Logger &logger = getInstance();
        logger.min_log_level_ = level;
    }

    LogLevel Logger::getLevel()
    {
        std::lock_guard<std::mutex> lock(_init_mutex);
        Logger &logger = getInstance();
        return logger.min_log_level_;
    }

    void Logger::writeLog(LogLevel level, const std::string &message)
    {
        if ( level < min_log_level_ ) {
            return; // Do not log messages below the minimum log level
        }

        std::lock_guard<std::mutex> lock(mutex_);
        if ( log_to_file_ ) {
            log_file_ << message << std::endl;
        } else {
            std::cerr << message << std::endl;
        }
    }

    // ================================
    // IMPLEMENTATION log_helper functions
    // ================================
    namespace log_helpers
    {
        std::string toString(LogLevel level)
        {
            switch ( level ) {
                case LogLevel::WARN:
                    return "WARN";
                case LogLevel::ERROR:
                    return "ERROR";
                case LogLevel::DEBUG:
                    return "DEBUG";
                case LogLevel::INFO:
                default:
                    return "INFO";
            }
        }

        std::string formatLevel(LogLevel level, bool log_to_file)
        {
            if ( log_to_file ) {
                return log_helpers::toString(level);
            }

            // console has colored logs
            switch ( level ) {
                case LogLevel::WARN:
                    return "\033[0;33m" + log_helpers::toString(level) + "\033[0m"; // yellow
                case LogLevel::ERROR:
                    return "\033[0;31m" + log_helpers::toString(level) + "\033[0m"; // red
                case LogLevel::DEBUG:
                    return "\033[0;34m" + log_helpers::toString(level) + "\033[0m"; // blue
                case LogLevel::INFO:
                default:
                    return "\033[0;37m" + log_helpers::toString(level) + "\033[0m"; // white
            }
        }

        std::string stripFilePath(const char *file)
        {
            std::string file_str(file);
            const std::string marker = "dominion/";
            auto pos = file_str.find(marker);
            if ( pos != std::string::npos ) {
                return file_str.substr(pos + marker.length());
            }
            return file_str; // return full path if marker was not found
        }

        std::string formatTimestamp()
        {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            std::ostringstream ss;
            ss << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0')
               << std::setw(3) << ms.count() << "]";
            return ss.str();
        }

        std::string formatLogLevel(LogLevel level, bool log_to_file)
        {
            const size_t level_width = (log_to_file ? 5 : 16);
            std::ostringstream ss;
            ss << "[" << std::left << std::setw(level_width) << formatLevel(level, log_to_file) << "]";
            return ss.str();
        }

        std::string formatFileLine(const char *file, int line)
        {
            std::ostringstream ss;
            ss << "[" << stripFilePath(file) << ":" << line << "]";
            return ss.str();
        }
    } // namespace log_helpers
} // namespace shared
