#pragma once
#include <memory>
#include <string>
#include <sys/time.h>

#include "FileWriterType.h"
// 文件追加 和 持久化
class FileWriter {
public:
    FileWriter() = default;
    virtual ~FileWriter() = default;
    virtual void append(const char *msg, int32_t len) = 0;
    virtual void flush() = 0;
    virtual uint32_t writtenBytes() const = 0;
};

// 记录文件路径，滚动尺度，持久化间隔，目的地文件类型
class LogFile {
public:
    LogFile(const std::string &basename, int32_t roll_size,
    int32_t flush_interval, int32_t check_interval,
    FileWriterType file_writer_type);
    ~LogFile();

    void append(const char *logline, int32_t len);
    void flush();
    bool rollFile();

private:
    std::string basename_; // 文件前缀名
    uint32_t roll_size_; // 滚动大小
    uint32_t flush_interval_; // 刷盘间隔 单位秒
    uint32_t check_freq_count_; // 
    uint32_t count_;
    time_t start_of_period_; // 文件开始的日期
    time_t last_roll_; // 上次滚动日志的时间
    time_t last_flush_;// 上次刷盘的时间
    std::shared_ptr<FileWriter> file_; // 文件指针
    FileWriterType file_writer_type_; // 文件类型
    constexpr static int kRollPerSeconds = 60 * 60 * 24; // 每日秒数
};