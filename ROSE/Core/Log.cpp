#include "Log.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>

LogLevel Log::s_Level = LogLevel::Debug;
std::ofstream Log::s_File;
std::mutex Log::s_Mutex;
bool Log::s_Initialized = false;

void Log::Init() {
    if (s_Initialized) return;
    s_File.open("rose_engine.log", std::ios::out | std::ios::trunc);
    s_Initialized = true;
    Info("ROSE Engine Logger initialized");
}

void Log::Shutdown() {
    if (s_File.is_open()) {
        s_File.close();
    }
    s_Initialized = false;
}

void Log::SetLevel(LogLevel level) {
    s_Level = level;
}

LogLevel Log::GetLevel() {
    return s_Level;
}

void Log::Trace(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMessage(LogLevel::Trace, fmt, args);
    va_end(args);
}

void Log::Debug(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMessage(LogLevel::Debug, fmt, args);
    va_end(args);
}

void Log::Info(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMessage(LogLevel::Info, fmt, args);
    va_end(args);
}

void Log::Warn(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMessage(LogLevel::Warning, fmt, args);
    va_end(args);
}

void Log::Error(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMessage(LogLevel::Error, fmt, args);
    va_end(args);
}

void Log::Fatal(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    LogMessage(LogLevel::Fatal, fmt, args);
    va_end(args);
}

const char* Log::LevelToString(LogLevel level) {
    switch (level) {
        case LogLevel::Trace:   return "[TRACE]";
        case LogLevel::Debug:   return "[DEBUG]";
        case LogLevel::Info:    return "[INFO]";
        case LogLevel::Warning: return "[WARN]";
        case LogLevel::Error:   return "[ERROR]";
        case LogLevel::Fatal:   return "[FATAL]";
        default: return "[?????]";
    }
}

void Log::LogMessage(LogLevel level, const char* fmt, va_list args) {
    if (level < s_Level) return;
    std::lock_guard<std::mutex> lock(s_Mutex);

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    localtime_s(&tm_buf, &time);

    char timeBuf[16];
    std::strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &tm_buf);

    std::string msg(256, '\0');
    int len = vsnprintf(&msg[0], msg.size(), fmt, args);
    if (len >= static_cast<int>(msg.size())) {
        msg.resize(len + 1);
        vsnprintf(&msg[0], msg.size(), fmt, args);
    }
    msg.resize(len);

    std::string output = std::string(timeBuf) + " " + LevelToString(level) + " " + msg;
    std::cout << output << std::endl;

    if (s_File.is_open()) {
        s_File << output << std::endl;
        s_File.flush();
    }
}

void Log::WriteToFile(const char* message) {
    if (s_File.is_open()) {
        s_File << message << std::endl;
        s_File.flush();
    }
}
