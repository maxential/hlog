#pragma once
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <ctime>
#include <fstream>
#include <string>

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
    constexpr const char* COLOR_DARK = "\033[90m"; // Red
    /*
        Forward function declarations
    */

    inline std::string get_log_filename();
   // inline void log(const char* format, ...);
    inline void log(const char* file, const char* line, int warning_level, const char* format, ...);
    inline std::string get_short_filename(const char* filepath);

    std::ofstream log_file;

    inline bool setup() {
        std::string filename = get_log_filename();
        log_file.open(filename);
        if (!log_file.is_open()) {
            log(WARNING_CRITICAL, "Failed to initialize hlog");
            return 1;
        }
        return 0;
    }

    // Function to get short filename (without path)
    inline std::string get_short_filename(const char* filepath) {
        std::string fullpath(filepath);
        size_t found = fullpath.find_last_of("/\\");
        if (found != std::string::npos) {
            return fullpath.substr(found + 1);
        }
        return fullpath;
    }

    inline std::string get_log_filename() {
        auto now = std::chrono::system_clock::now();
        std::time_t time = std::chrono::system_clock::to_time_t(now);

        std::tm local_tm;
#ifdef _WIN32
        // Use localtime_s on Windows
        localtime_s(&local_tm, &time);
#else
        // Use localtime_r on POSIX systems
        localtime_r(&time, &local_tm);
#endif

        char buffer[80];
        std::strftime(buffer, 80, "log_%Y%m%d_%H%M%S.txt", &local_tm);

        return std::string(buffer);
    }
    
    /*
        Main logging function
    */

    inline void log_impl(const char* file, const char* line, int warning_level, const char* format, va_list args)
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

        // Print the warning level and set the color to console
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

        printf("%s (%s %d)%s ",COLOR_DARK, get_short_filename(__FILE__).c_str(), __LINE__,COLOR_NONE);
        // Print to file
        if (hlog::log_file.is_open()) {
            switch (warning_level)
            {
            case WARNING_NONE:
                hlog::log_file << "[HLOG]     ";
                break;
            case WARNING_SUCCESS:
                hlog::log_file << "[SUCCESS]  ";
                break;
            case WARNING_ERROR:
                hlog::log_file << "[ERROR]    ";
                break;
            case WARNING_CRITICAL:
                hlog::log_file << "[CRITICAL] ";
                break;
            default:
                hlog::log_file << "[" << warning_level << "] ";
                break;
            }

            char buffer[1024];
            vsnprintf(buffer, sizeof(buffer), format, args);

            // Write to file
            log_file << " (" << get_short_filename(__FILE__).c_str() << " " << __LINE__ << ") " << buffer  << std::endl;
        }

        vprintf(format, args);

        printf("%s\n", COLOR_DEFAULT);
    }

    /*
        Function overload for laziness
    */

    // Variadic log function with warning level
    //inline void log(const char* format, ...)
    //{
    //    va_list args;
    //    va_start(args, format);
    //    log_impl(WARNING_NONE, format, args);
    //    va_end(args);
    //}

    // Variadic log function with custom warning level
    inline void log(const char* file, const char* line,int warning_level, const char* format, ...)
    {
        va_list args;
        va_start(args, format);
        log_impl(file,line,warning_level, format, args);
        va_end(args);
    }

}

#define HLOG(...) hlog::log(__FILE__,__LINE__,hlog::WARNING_NONE, __VA_ARGS__)
#define HLOG_SUCCESS(...) hlog::log(__FILE__,__LINE__,hlog::WARNING_SUCCESS, __VA_ARGS__)
#define HLOG_ERROR(...) hlog::log(__FILE__,__LINE__,hlog::WARNING_ERROR, __VA_ARGS__)
#define HLOG_CRITICAL(...) hlog::log(__FILE__,__LINE__,hlog::WARNING_CRITICAL, __VA_ARGS__)