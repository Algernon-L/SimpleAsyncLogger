#pragma once
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include "CountDownLatch.h"
#include "LogAppenderInterface.h"
#include "FileWriterType.h"

class LogBuffer;

class AsyncFileAppender : public LogAppenderInterface {
public:
    AsyncFileAppender(const std::string &basename, const FileWriterType fileType);
    ~AsyncFileAppender();
    void append(const char *msg, size_t len);
    void start();
    void stop();

private:
    void threadFunc();
    bool started_;
    bool running_;
    uint32_t persist_period_;// 持久化周期
    std::string basename_;
    FileWriterType fileType_;
    std::mutex mutex_;
    std::condition_variable cv_;
    CountDownLatch countdownlatch_;
    std::thread persit_thread_;// 持久化线程
    std::unique_ptr<LogBuffer> buffer_writing_;// 前端线程正在写入的buf
    std::vector<std::unique_ptr<LogBuffer>> buffers_written_;// 前端线程已写好的buf
};