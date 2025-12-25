//
//  Logger.cpp
//  FR-Ocean Engine
//
//  Implementation of the logging system.
//

#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

void Logger::Init(const std::string& logFilePath) {
    std::lock_guard<std::mutex> lock(log_mutex);

    if (initialized) {
        return;
    }

    initialized = true;

    if (!logFilePath.empty()) {
        log_file.open(logFilePath, std::ios::out | std::ios::app);
        file_logging_enabled = log_file.is_open();
    }
}

void Logger::Shutdown() {
    std::lock_guard<std::mutex> lock(log_mutex);

    if (!initialized) {
        return;
    }

    if (file_logging_enabled && log_file.is_open()) {
        log_file.close();
    }

    file_logging_enabled = false;
    initialized = false;
}

void Logger::Log(LogLevel level, const std::string& message,
                 const char* file, int line) {
    if (level < min_level) {
        return;
    }

    std::lock_guard<std::mutex> lock(log_mutex);

    std::string timestamp = GetTimestamp();
    std::string levelStr = LevelToString(level);

    // Build location string
    std::string location;
    if (file != nullptr && line > 0) {
        location = " [" + ExtractFilename(file) + ":" + std::to_string(line) + "]";
    }

    // Format: [TIMESTAMP] [LEVEL] message [file:line]
    std::string formattedMessage = "[" + timestamp + "] [" + levelStr + "] " + message + location;

    // Console output
    std::ostream& out = (level >= LogLevel::ERROR) ? std::cerr : std::cout;

    if (colored_output) {
        out << GetColorCode(level) << formattedMessage << GetResetCode() << std::endl;
    } else {
        out << formattedMessage << std::endl;
    }

    // File output
    if (file_logging_enabled && log_file.is_open()) {
        log_file << formattedMessage << std::endl;
        log_file.flush();
    }
}

void Logger::SetMinLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(log_mutex);
    min_level = level;
}

LogLevel Logger::GetMinLevel() {
    std::lock_guard<std::mutex> lock(log_mutex);
    return min_level;
}

bool Logger::EnableFileLogging(const std::string& path) {
    std::lock_guard<std::mutex> lock(log_mutex);

    if (file_logging_enabled && log_file.is_open()) {
        log_file.close();
    }

    log_file.open(path, std::ios::out | std::ios::app);
    file_logging_enabled = log_file.is_open();

    return file_logging_enabled;
}

void Logger::DisableFileLogging() {
    std::lock_guard<std::mutex> lock(log_mutex);

    if (file_logging_enabled && log_file.is_open()) {
        log_file.close();
    }

    file_logging_enabled = false;
}

void Logger::SetColoredOutput(bool enabled) {
    std::lock_guard<std::mutex> lock(log_mutex);
    colored_output = enabled;
}

std::string Logger::GetTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;

    std::tm tm_buf;
#ifdef _WIN32
    localtime_s(&tm_buf, &time);
#else
    localtime_r(&time, &tm_buf);
#endif

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    return oss.str();
}

std::string Logger::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO ";
        case LogLevel::WARNING: return "WARN ";
        case LogLevel::ERROR:   return "ERROR";
        case LogLevel::FATAL:   return "FATAL";
        default:                return "?????";
    }
}

std::string Logger::GetColorCode(LogLevel level) {
#ifdef _WIN32
    // Windows doesn't support ANSI codes in all terminals
    return "";
#else
    switch (level) {
        case LogLevel::DEBUG:   return "\033[36m";  // Cyan
        case LogLevel::INFO:    return "\033[32m";  // Green
        case LogLevel::WARNING: return "\033[33m";  // Yellow
        case LogLevel::ERROR:   return "\033[31m";  // Red
        case LogLevel::FATAL:   return "\033[35m";  // Magenta
        default:                return "";
    }
#endif
}

std::string Logger::GetResetCode() {
#ifdef _WIN32
    return "";
#else
    return "\033[0m";
#endif
}

std::string Logger::ExtractFilename(const char* path) {
    if (path == nullptr) {
        return "";
    }

    std::string fullPath(path);

    // Find last separator
    size_t lastSlash = fullPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        return fullPath.substr(lastSlash + 1);
    }

    return fullPath;
}
