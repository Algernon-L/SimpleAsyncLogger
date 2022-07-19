#include "Logger.h"
#include "LogConfig.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <thread>

// 线程局部变量
thread_local char t_time[64];
thread_local time_t t_lastSecond;
// 日志级别字符串
const char* LogLevelName[6] =
{
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

// 编译时确定字符串长度
class T
{
public:
    T(const char* str, unsigned len)
        :str_(str),
        len_(len)
    {}
    const char* str_;
    const unsigned len_;
};

// LogStream << 输出到LogStream的缓冲区中
inline LogStream& operator<<(LogStream& s, T v)
{
    s.append(v.str_, v.len_);
    return s;
}

inline LogStream& operator<<(LogStream& s, const Logger::SourceFile& v)
{
    s.append(v.data_, v.size_);
    return s;
}

// 构造一个Impl用于每条日志的输出
Logger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line)
  : time_(LogTimestamp::now()),
    stream_(),
    level_(level),
    line_(line),
    basename_(file)
{
    formatTime();
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    std::string tidstring = oss.str();
    stream_ << T(tidstring.data(), tidstring.size());
    stream_ << ' ';
    stream_ << T(LogLevelName[level], 6);
}

// 格式化当前时间并输出到stream buf中
void Logger::Impl::formatTime()
{
    int64_t microSecondsSinceEpoch = time_.getMicroSecondsFromEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / LogTimestamp::kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % LogTimestamp::kMicroSecondsPerSecond);
    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
        ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime

        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }

    Fmt us(".%06d ", microseconds);
    stream_ << T(t_time, 17) << T(us.data(), 8);
}

void Logger::Impl::finish()
{
    stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(SourceFile file, int line, LogLevel level)
  : impl_(level, 0, file, line)
{
}

// 析构时输出到buffer
Logger::~Logger()
{
    impl_.finish();
    const LogStream::Buffer& buf(stream().buffer());
    for(auto iter = LogConfig::getInstance().destinationmap.begin();
     iter != LogConfig::getInstance().destinationmap.end(); iter++)
        iter->second->append(buf.data(), buf.length());
}