#pragma once
#include "Timestamp.h"
#include "LogStream.h"
#include "LogConfig.h"
#include "LogAppenderInterface.h"
#include "LogLevel.h"
#include <unordered_map>
class LogStream;
class Timestamp;
class Logger
{
public:
    class SourceFile
    {
    public:
        template<int N>
        SourceFile(const char (&arr)[N])
            : data_(arr),
            size_(N-1)
        {
            const char* slash = strrchr(data_, '/'); // builtin function
            if (slash)
            {
            data_ = slash + 1;
            size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char* filename)
            : data_(filename)
        {
            const char* slash = strrchr(filename, '/');
            if (slash)
            {
            data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*FlushFunc)();
    static void setFlush(FlushFunc);

private:
    class Impl{
    public:
        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
        void formatTime();
        void finish();

        Timestamp time_;
        LogStream stream_;
        LogLevel level_;
        int line_;
        SourceFile basename_;
    };
    Impl impl_;
};

#define LOG_TRACE if (LogConfig::getInstance().log_level <= LogLevel::TRACE) \
  Logger(__FILE__, __LINE__, LogLevel::TRACE).stream()
#define LOG_DEBUG if (LogConfig::getInstance().log_level <= LogLevel::DEBUG) \
  Logger(__FILE__, __LINE__, LogLevel::DEBUG).stream()
#define LOG_INFO if (LogConfig::getInstance().log_level <= LogLevel::INFO) \
  Logger(__FILE__, __LINE__, LogLevel::INFO).stream()
#define LOG_WARN Logger(__FILE__, __LINE__, Logger::WARN).stream()
#define LOG_ERROR Logger(__FILE__, __LINE__, Logger::ERROR).stream()
#define LOG_FATAL Logger(__FILE__, __LINE__, Logger::FATAL).stream()
