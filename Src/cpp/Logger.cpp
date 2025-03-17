#include "Logger.h"

#include <cstdarg>
#include <string>

/*static*/ void Logger::log(LogLevel level, const char* format, ...) {
        // Select color based on log level (ANSI escape codes for terminal output)
        const char* color;
        switch (level) {
            case LogLevel::ERROR: color = "\033[31m"; break;  // Red
            case LogLevel::DEBUG: color = "\033[34m"; break;  // Blue
            case LogLevel::INFO: color = "\033[32m"; break;   // Green
            case LogLevel::WARNING: color = "\033[33m"; break;  // Yellow
        }

        // Print the color for log level
        printf("%s", color);

        // Print log level text
        switch (level) {
            case LogLevel::ERROR: printf("[ERROR] "); break;
            case LogLevel::DEBUG: printf("[DEBUG] "); break;
            case LogLevel::INFO: printf("[INFO] "); break;
            case LogLevel::WARNING: printf("[WARNING] "); break;
        }

        // Process variable arguments
        va_list args;
        va_start(args, format);
        vprintf(format, args);  // Use vprintf to handle the variable arguments
        va_end(args);

        // Reset color
        printf("\033[0m\n");
    }