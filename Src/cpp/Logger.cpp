#include "Logger.h"

#include <cstdarg>
#include <string>
#include <regex>


std::string getFilenameFromPath(const std::string& fullPath) {
    size_t pos = fullPath.find_last_of("/\\");
    if (pos != std::string::npos) {
        return fullPath.substr(pos + 1);
    }
    return fullPath;  // No slashes found, return the original string
}

/*static*/ void Logger::log(LogLevel level, const char *file, int line, const char *format, ...) {
    const char *color;
    switch (level) {
        case LogLevel::ERROR:
            color = "\033[31m";
            break;  // Red
        case LogLevel::DEBUG:
            color = "\033[34m";
            break;  // Blue
        case LogLevel::INFO:
            color = "\033[32m";
            break;   // Green
        case LogLevel::WARNING:
            color = "\033[33m";
            break;  // Yellow
    }

    printf("%s", color);

    switch (level) {
        case LogLevel::ERROR:
            printf("[ERROR] ");
            break;
        case LogLevel::DEBUG:
            printf("[DEBUG] ");
            break;
        case LogLevel::INFO:
            printf("[INFO] ");
            break;
        case LogLevel::WARNING:
            printf("[WARNING] ");
            break;
    }

    printf("%s:%d ", getFilenameFromPath(file).c_str(), line);

    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Use vprintf to handle the variable arguments
    va_end(args);

    printf("\033[0m\n");
}
