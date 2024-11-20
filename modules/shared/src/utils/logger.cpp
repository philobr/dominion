
#include <shared/utils/logger.h>

#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace shared
{
    // ================================
    // IMPLEMENTATION LogStream
    // ================================
    Logger::LogStream::LogStream(Logger &logger, LogLevel level, const char *file, int line) : logger_(logger)
    {
        ss_ << log_helpers::formatTimestamp() << " ";
        ss_ << log_helpers::formatLogLevel(level, logger.log_to_file_) << " ";
        ss_ << log_helpers::formatFileLine(file, line) << " - ";
    }

    // ================================
    // IMPLEMENTATION Logger
    // ================================

    // constructor (this is only here to not include the comment above as function description)
    Logger::Logger(const std::string &file_path) : log_to_file_(!file_path.empty())
    {
        if ( log_to_file_ ) {
            std::filesystem::path log_path = file_path;
            std::filesystem::create_directories(log_path.parent_path());

            log_file_.open(file_path, std::ios::out | std::ios::app);
            if ( !log_file_ ) {
                throw exception::Logger("Failed to open log file: " + file_path);
            }
        }
    }

    Logger::~Logger()
    {
        if ( log_file_.is_open() ) {
            if ( log_to_file_ ) {
                log_file_ << "[INFO] - END LOG" << std::endl;
            }
            log_file_.close();
        }
    }

    void shared::Logger::initialize(const std::string &file_path)
    {
        std::lock_guard<std::mutex> lock(init_mutex_);
        if ( !instance_ ) {
            if ( file_path.empty() ) {
                instance_ = std::unique_ptr<Logger>(new Logger());
                LOG(LogLevel::INFO) << "Logging to std::cerr.";
            } else {
                const std::string default_directory = std::string(PROJECT_ROOT) + "/" + std::string("build/logs");
                const std::string full_path = default_directory + "/" + file_path;

                instance_ = std::unique_ptr<Logger>(new Logger(full_path));
                std::cerr << "Logging to file: build/logs/" + file_path << std::endl;
            }
        } else {
            throw exception::Logger("Logger has already been initialized.");
        }
    }

    Logger &Logger::getInstance()
    {
        if ( !instance_ ) {
            initialize(); // default init logs to console
            LOG(LogLevel::WARN) << "Logger not initialised";
        }

        return *instance_;
    }

    void Logger::writeLog(const std::string &message)
    {
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
