#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <string>

#define LOG(level) Logger::getInstance().log(level, __FILE__, __LINE__).stream()

enum LogLevel
{
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_DEBUG
};

class Logger
{
public:
    static void initialize(const std::string &filePath)
    {
        std::lock_guard<std::mutex> lock(initMutex_);
        if ( !instance_ ) {
            instance_ = std::unique_ptr<Logger>(new Logger(filePath));
        } else {
            throw std::runtime_error("Logger has already been initialized.");
        }
    }

    // Access the singleton instance
    static Logger &getInstance()
    {
        if ( !instance_ ) {
            throw std::runtime_error("Logger has not been initialized. Call Logger::initialize() first.");
        }
        return *instance_;
    }

    // Logging function
    class LogStream
    {
    public:
        LogStream(Logger &logger, LogLevel level, const char *file, int line) :
            logger_(logger), level_(level), file_(file), line_(line)
        {
            // Add timestamp
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

            ss_ << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "." << std::setfill('0')
                << std::setw(3) << ms.count() << "] ";

            // Add log level
            ss_ << "[" << logger.logLevelToString(level_) << "] ";

            // Add file and line number
            ss_ << file << ":" << line << " - ";
        }

        ~LogStream() { logger_.writeLog(ss_.str()); }

        std::ostringstream &stream() { return ss_; }

    private:
        Logger &logger_;
        LogLevel level_;
        const char *file_;
        int line_;
        std::ostringstream ss_;
    };

    LogStream log(LogLevel level, const char *file, int line) { return LogStream(*this, level, file, line); }

private:
    // Singleton instance and mutex for initialization
    static std::unique_ptr<Logger> instance_;
    static std::mutex initMutex_;

    std::mutex mutex_; // For thread-safe logging
    std::ofstream logFile_; // Optional log file

    // Private constructor to enforce singleton
    explicit Logger(const std::string &filePath)
    {
        logFile_.open(filePath, std::ios::out | std::ios::app);
        if ( !logFile_ ) {
            throw std::runtime_error("Failed to open log file: " + filePath);
        }
    }

    ~Logger()
    {
        if ( logFile_.is_open() ) {
            logFile_.close();
        }
    }

    // Non-copyable and non-movable
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

    void writeLog(const std::string &message)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if ( logFile_.is_open() ) {
            logFile_ << message << std::endl;
        } else {
            std::cout << message << std::endl;
        }
    }

    std::string logLevelToString(LogLevel level) const
    {
        switch ( level ) {
            case LOG_INFO:
                return "INFO";
            case LOG_WARN:
                return "WARN";
            case LOG_ERROR:
                return "ERROR";
            case LOG_DEBUG:
                return "DEBUG";
            default:
                return "UNKNOWN";
        }
    }
};

// Define static members
std::unique_ptr<Logger> Logger::instance_ = nullptr;
std::mutex Logger::initMutex_;
