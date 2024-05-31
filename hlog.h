#pragma once
#include <cstdarg>
#include <cstdio>

#ifdef _WIN32
#include <windows.h>
#endif

namespace hlog
{
    /*
        Warning levels
    */

    constexpr int WARNING_NONE = 0;
    constexpr int WARNING_SUCCESS = 1;
    constexpr int WARNING_ERROR = 2;
    constexpr int WARNING_CRITICAL = 3;

    /*
        Color codes
    */
    constexpr const char* COLOR_DEFAULT = "\033[0m";   // Grey
    constexpr const char* COLOR_NONE = "\033[97m";     // White
    constexpr const char* COLOR_SUCCESS = "\033[32m";  // Green
    constexpr const char* COLOR_ERROR = "\033[33m";    // Yellow
    constexpr const char* COLOR_CRITICAL = "\033[31m"; // Red
    
    /*
        Main logging function
    */

    inline void log_impl(int warning_level, const char* format, va_list args)
    {
#ifdef _WIN32
        // Enable ANSI escape codes in Windows Command Prompt
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode = 0;
        if (hConsole == INVALID_HANDLE_VALUE || !GetConsoleMode(hConsole, &mode)) {
            return;
        }
        SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
        // Print the warning level and set the color
        switch (warning_level)
        {
        case WARNING_NONE:
            printf("%s[HLOG]     ", COLOR_NONE);
            break;
        case WARNING_SUCCESS:
            printf("%s[SUCCESS] %s ", COLOR_SUCCESS, COLOR_NONE);
            break;
        case WARNING_ERROR:
            printf("%s[ERROR] %s   ", COLOR_ERROR, COLOR_NONE);
            break;
        case WARNING_CRITICAL:
            printf("%s[CRITICAL] %s", COLOR_CRITICAL, COLOR_NONE);
            break;
        default:
            printf("%s[UNKNOWN]    ", COLOR_NONE);
            break;
        }

        // Print the formatted string
        vprintf(format, args);

        // Print a new line and reset color to default
        printf("%s\n", COLOR_DEFAULT);
    }

    /*
        Function overload for laziness
    */

    // Variadic log function with warning level
    inline void log(const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        log_impl(WARNING_NONE, format, args);
        va_end(args);
    }

    // Variadic log function with custom warning level
    inline void log(int warning_level, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        log_impl(warning_level, format, args);
        va_end(args);
    }
}

