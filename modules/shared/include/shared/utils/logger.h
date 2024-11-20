/**!
 * @file
 * @brief This is our logger. It provides a few useful macros which can be used to either log to a file or to log to
 * console. If the logger is not initialised then we default to 'std::cerr'. A logger can only be instantiated once!
 *
 * @warning you should only use the LOG(leve) macro from this file!
 */

#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>

#include <shared/utils/exception.h>

// this _should_ be defined in the root CMakeLists.txt, but you never know
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "/default/path/for/linter"
#endif

#ifdef NDEBUG // debug messages are only printed when debugging is actually enabled
/**
 * @brief This macro returns a stream with the desired level. If NDEBUG is set we will not log debug messages
 */
#define LOG(level)                                                                                                     \
    if ( (level) == LogLevel::DEBUG ) {                                                                                \
    } else                                                                                                             \
        shared::Logger::getInstance().log(level, __FILE__, __LINE__).stream()
#else
/**
 * @brief This macro returns a stream with the desired level. If NDEBUG is set we will not log debug messages
 */
#define LOG(level) shared::Logger::getInstance().log(level, __FILE__, __LINE__).stream()
#endif

// not in shared namespace to simplify usage
enum LogLevel
{
    INFO,
    WARN,
    ERROR,
    DEBUG
};

namespace shared
{
    namespace log_helpers
    {
        std::string toString(LogLevel level);
        std::string formatLevel(LogLevel level, bool toggle_colors);

        std::string formatLogLevel(LogLevel level, bool toggle_colors);
        std::string formatFileLine(const char *file, int line);
        std::string formatTimestamp();

        /**
         * @brief Strips the filepath to be rooted in our root directory
         * @param file
         * @return std::string
         */
        std::string stripFilePath(const char *file);

    } // namespace log_helpers
} // namespace shared


namespace shared
{
    class Logger
    {
        class LogStream
        {
        public:
            LogStream(Logger &logger, LogLevel level, const char *file, int line);
            ~LogStream() { logger_.writeLog(ss_.str()); }

            std::ostringstream &stream() { return ss_; }

        private:
            Logger &logger_;
            std::ostringstream ss_;
        };

    public:
        /**
         * @brief If no file_path is set we will default to std::cout. Will automatically add a prefix
         * (build/logs/file_path.log) to the output path
         * @param file_path filename.log
         */
        static void initialize(const std::string &file_path = "");

        /**
         * @brief Returns an instance to a logger. If none is found we default to a std::cerr instance.
         *
         * @return Logger&
         */
        static Logger &getInstance();

        ~Logger();

        LogStream log(LogLevel level, const char *file, int line) { return LogStream(*this, level, file, line); }

    private:
        inline static std::mutex init_mutex_;
        inline static std::unique_ptr<Logger> instance_;

        std::mutex mutex_;
        std::ofstream log_file_;
        bool log_to_file_;

        explicit Logger(const std::string &file_path = "");

        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        void writeLog(const std::string &message);
    };

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
                std::cout << "Logging to file: build/logs/" + file_path << std::endl;
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