#pragma once

#include <cstdarg>
#include <string>

/**
 * @brief Log levels for the logger.
 */
enum class LogLevel {
    ERROR,
    WARNING,
    DEBUG,
    INFO
};

class Logger {
public:
    /**
     * @brief Logs a message with the specified log level.
     * @param level Log level.
     * @param format Format string.
     * @param ... Additional arguments.
     */
    static void log(LogLevel level, const char* file, int line, const char* format, ...);
};
