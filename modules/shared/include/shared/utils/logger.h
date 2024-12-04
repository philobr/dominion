#pragma once

#include <fstream>
#include <memory>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>

#include <shared/utils/exception.h>
#include <shared/utils/utils.h>

// this _should_ be defined in the root CMakeLists.txt, but you never know
#ifndef PROJECT_ROOT
#define PROJECT_ROOT "/default/path/for/linter"
#endif

/**
 * @brief This macro returns a stream with the desired level.
 */
#define LOG(level) shared::Logger::getInstance().log(level, __FILE__, __LINE__).stream()

/**
 * @brief Returns a string with the cleaned up class name
 */
#define CLASS_NAME (std::string("{class:") + utils::demangle(typeid(*this).name()) + std::string("}"))

/**
 * @brief Returns a string with the cleaned up function name
 */
#define FUNC_NAME (std::string("{func:") + utils::demangle(__func__) + std::string("}"))

#define LOG_FUNCTION_ENTRY LOG(INFO) << "ENTERED: " << FUNC_NAME
#define LOG_FUNCTION_EXIT LOG(INFO) << "EXITED: " << FUNC_NAME

// not in shared namespace to simplify usage
enum LogLevel
{
    INFO, // everything
    DEBUG, // important stuff
    WARN, // unexpected, but handled behaviour
    ERROR // actual errors
};

std::ostream &operator<<(std::ostream &os, const LogLevel &level);

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
    /**
     * @brief Parses a string to a LogLevel.
     */
    std::optional<LogLevel> parseLogLevel(const std::string &level);

    class Logger
    {
        class LogStream
        {
        public:
            LogStream(Logger &logger, LogLevel level, const char *file, int line);
            ~LogStream();

            std::ostringstream &stream() { return ss_; }

        private:
            Logger &logger_;
            LogLevel level_;
            std::ostringstream ss_;
        };

    public:
        /**
         * @brief Initializes the logger. If not called, logging will default to std::cerr.
         */
        static void initialize();

        /**
         * @brief Sets the file path for logging output.
         * @param file_path The full file path where logs will be written.
         */
        static void writeTo(const std::string &file_path = "");

        /**
         * @brief Sets the minimum log level. Messages below this level will not be logged.
         * @param level The minimum LogLevel to log.
         */
        static void setLevel(LogLevel level);

        /**
         * @brief Returns the current minimum log level.
         */
        static LogLevel getLevel();

        /**
         * @brief Returns an instance to the logger.
         *
         * @return Logger&
         */
        static Logger &getInstance();

        ~Logger();

        LogStream log(LogLevel level, const char *file, int line) { return LogStream(*this, level, file, line); }

    private:
        inline static std::mutex _init_mutex;
        inline static std::unique_ptr<Logger> _instance;

        LogLevel min_log_level_;

        std::mutex mutex_;
        std::ofstream log_file_;
        bool log_to_file_;

        Logger();

        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        void writeLog(LogLevel level, const std::string &message);
    };
} // namespace shared
