#ifndef LOG_H
#define LOG_H

#include <string>
#include <fstream>
#include <mutex>
#include <cstdarg>

enum class LogLevel { Trace = 0, Debug = 1, Info = 2, Warning = 3, Error = 4, Fatal = 5 };

class Log {
public:
    static void Init();
    static void Shutdown();
    static void SetLevel(LogLevel level);
    static LogLevel GetLevel();

    static void Trace(const char* fmt, ...);
    static void Debug(const char* fmt, ...);
    static void Info(const char* fmt, ...);
    static void Warn(const char* fmt, ...);
    static void Error(const char* fmt, ...);
    static void Fatal(const char* fmt, ...);

    static const char* LevelToString(LogLevel level);

private:
    static void LogMessage(LogLevel level, const char* fmt, va_list args);
    static void WriteToFile(const char* message);

    static LogLevel s_Level;
    static std::ofstream s_File;
    static std::mutex s_Mutex;
    static bool s_Initialized;
};

#define ROSE_LOG_TRACE(...)    Log::Trace(__VA_ARGS__)
#define ROSE_LOG_DEBUG(...)    Log::Debug(__VA_ARGS__)
#define ROSE_LOG_INFO(...)     Log::Info(__VA_ARGS__)
#define ROSE_LOG_WARN(...)     Log::Warn(__VA_ARGS__)
#define ROSE_LOG_ERROR(...)    Log::Error(__VA_ARGS__)
#define ROSE_LOG_FATAL(...)    Log::Fatal(__VA_ARGS__)

#endif
