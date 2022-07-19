#pragma once

#include <stdint.h>
#include "FileWriterType.h"
#include "LogLevel.h"
#include <string>
#include "AsyncFileAppender.h"
#include <unordered_map>
#include <mutex>
class LogAppenderInterface;

class LogConfig{
public:
    static LogConfig& getInstance(){
        static LogConfig instance;
        return instance;
    }
    LogConfig(const LogConfig&) = delete;
    LogConfig& operator=(const LogConfig&) = delete;

    uint32_t log_buffer_size;
    uint32_t log_buffer_nums;
    LogLevel log_level;
    std::string file_path;
    //日志滚动大小和时间间隔，仅限asyncfileappender
    uint32_t log_flush_file_size;
    uint32_t log_flush_interval;
    void addAppender(const std::string &appender_name,
                LogAppenderInterface::Ptr appender);
    void delAppender(const std::string &appender_name);
    void clearAppender();
    std::unordered_map<std::string, LogAppenderInterface::Ptr> destinationmap;
    std::mutex mtx;
private:
    LogConfig():
        log_buffer_size(65536),//size都是以字节为单位
        log_buffer_nums(2),
        log_level(LogLevel::INFO),
        log_flush_file_size(65536),
        log_flush_interval(3)
        {};
};

