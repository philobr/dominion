/**!
 * @file
 * @brief This is our logger. It provides a few useful macros which can be used to either log to a file or to log to
 * console. If the logger is not initialised then we default to 'std::cerr'. A logger can only be instantiated once!
 *
 * @warning you should only use the LOG(leve) macro from this file!
 */

#pragma once

#include <memory>
#include <fstream>
#include <sstream>
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

} // namespace shared