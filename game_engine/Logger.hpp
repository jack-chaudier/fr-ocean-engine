//
//  Logger.hpp
//  FR-Ocean Engine
//
//  Logging system for engine diagnostics and error reporting.
//  Supports multiple log levels, console and file output, and automatic
//  source location tracking via macros.
//

#ifndef Logger_hpp
#define Logger_hpp

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>

/**
 * @enum LogLevel
 * @brief Severity levels for log messages.
 */
enum class LogLevel {
    DEBUG,      ///< Detailed debugging information
    INFO,       ///< General informational messages
    WARNING,    ///< Potential issues that don't prevent operation
    ERROR,      ///< Errors that may affect functionality
    FATAL       ///< Critical errors that will terminate the engine
};

/**
 * @class Logger
 * @brief Thread-safe logging system with configurable output destinations.
 *
 * The Logger class provides a centralized logging facility for the engine.
 * It supports multiple log levels, console output with optional colors,
 * and file output for persistent logging.
 *
 * Usage:
 * @code
 * Logger::Init();
 * LOG_INFO("Engine starting");
 * LOG_ERROR("Failed to load resource: " + resourceName);
 * Logger::Shutdown();
 * @endcode
 *
 * @note All methods are thread-safe.
 */
class Logger {
public:
    /**
     * @brief Initialize the logging system.
     * @param logFilePath Optional path to log file. Empty string disables file logging.
     */
    static void Init(const std::string& logFilePath = "");

    /**
     * @brief Shutdown the logging system and flush all outputs.
     */
    static void Shutdown();

    /**
     * @brief Log a message with the specified level and source location.
     * @param level Severity level of the message.
     * @param message The message to log.
     * @param file Source file name (use __FILE__ macro).
     * @param line Source line number (use __LINE__ macro).
     */
    static void Log(LogLevel level, const std::string& message,
                    const char* file = nullptr, int line = 0);

    /**
     * @brief Set the minimum log level for output.
     * Messages below this level will be suppressed.
     * @param level Minimum level to output.
     */
    static void SetMinLevel(LogLevel level);

    /**
     * @brief Get the current minimum log level.
     * @return Current minimum log level.
     */
    static LogLevel GetMinLevel();

    /**
     * @brief Enable logging to a file.
     * @param path Path to the log file.
     * @return true if file was opened successfully, false otherwise.
     */
    static bool EnableFileLogging(const std::string& path);

    /**
     * @brief Disable file logging and close any open log file.
     */
    static void DisableFileLogging();

    /**
     * @brief Enable or disable colored console output.
     * @param enabled true to enable colors, false to disable.
     */
    static void SetColoredOutput(bool enabled);

private:
    inline static LogLevel min_level = LogLevel::INFO;
    inline static std::ofstream log_file;
    inline static bool file_logging_enabled = false;
    inline static bool colored_output = true;
    inline static bool initialized = false;
    inline static std::mutex log_mutex;

    static std::string GetTimestamp();
    static std::string LevelToString(LogLevel level);
    static std::string GetColorCode(LogLevel level);
    static std::string GetResetCode();
    static std::string ExtractFilename(const char* path);
};

// Convenience macros with automatic file/line tracking
#define LOG_DEBUG(msg)   Logger::Log(LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg)    Logger::Log(LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_WARNING(msg) Logger::Log(LogLevel::WARNING, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg)   Logger::Log(LogLevel::ERROR, msg, __FILE__, __LINE__)
#define LOG_FATAL(msg)   Logger::Log(LogLevel::FATAL, msg, __FILE__, __LINE__)

#endif /* Logger_hpp */
