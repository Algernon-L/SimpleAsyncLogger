#include "AsyncFileAppender.h"

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include "Logger.h"
#include "LogBuffer.h"
#include "LogFile.h"
#include <chrono>
#include <functional>

// 异步写日志
// 初始化持久化线程
// 持久化线程启动 负责主循环
// 前端buffer初始化
// persit_thread() -> start() -> pthread_create-> cb -> threadFunc
AsyncFileAppender::AsyncFileAppender(const std::string &basename, const FileWriterType fileType):
    started_(false), 
    running_(false),
    persist_period_(LogConfig::getInstance().log_flush_interval),
    basename_(basename), 
    fileType_(fileType),
    cv_(), 
    countdownlatch_(1),
    persit_thread_(std::bind(&AsyncFileAppender::threadFunc, this)),
    buffer_writing_(new LogBuffer(LogConfig::getInstance().log_buffer_size)) 
{
    mkdir(basename_.c_str(), 0755);
    started_ = true;
    running_ = true;
    countdownlatch_.wait();
}

AsyncFileAppender::~AsyncFileAppender() {
    if (started_) {
        stop();
    }
}

// 前端线程执行append
void AsyncFileAppender::append(const char *msg, size_t len) {
    std::lock_guard<std::mutex> lg(mutex_);
    // 足够空间，直接写入
    if (buffer_writing_->available() >= len) {
        buffer_writing_->append(msg, len);
    //空间不够
    } else {
        buffers_written_.push_back(std::move(buffer_writing_));
        buffer_writing_.reset(new LogBuffer(LogConfig::getInstance().log_buffer_size));
        buffer_writing_->append(msg, len);
        cv_.notify_one();// buffers_written_有内容了，通知后端可写
    }
}

// 主线程摧毁后端线程
void AsyncFileAppender::stop() {
    started_ = false;
    cv_.notify_one();
    persit_thread_.join();
}

// 后端线程执行主循环
void AsyncFileAppender::threadFunc() {

    // 用于快速得到一个新的buffer_writing
    std::unique_ptr<LogBuffer> buffer_spare(new LogBuffer(LogConfig::getInstance().log_buffer_size));
    // 用于后端线程持久化数据
    std::vector<std::unique_ptr<LogBuffer>> buffers_persist;
    buffers_persist.reserve(LogConfig::getInstance().log_buffer_nums);
    // 新建日志文件
    LogFile log_file(
        basename_, LogConfig::getInstance().log_flush_file_size,
        LogConfig::getInstance().log_flush_interval, 1024,
        fileType_);
    countdownlatch_.countDown();

    // 主循环
    while (running_) {
        {
            std::unique_lock<std::mutex> lk(mutex_);
            // 后端没有东西，等待
            if (buffers_written_.empty()) {
                using namespace std::chrono_literals;
                cv_.wait_for(lk, persist_period_ * 1s);
            }
            // 还是没有，并且前端也没有，跳过，继续等待
            if (buffers_written_.empty() && buffer_writing_->length() == 0) {
                continue;
            }
            
            // 将前端数据移动到后端
            buffers_written_.push_back(std::move(buffer_writing_));
                                              
            // 重置前端线程使用的buffer
            buffers_persist.swap(buffers_written_);
            buffer_writing_ = std::move(buffer_spare);
            buffer_writing_->clear();
        }

        // 太多日志了，丢掉一些
        if (buffers_persist.size() > LogConfig::getInstance().log_buffer_size) {
            std::cerr << "log is too large, drop some" << std::endl;
            buffers_persist.erase(buffers_persist.begin() + 1, buffers_persist.end());
        }

        // 写入日志到文件
        for (const auto &persist_buffer : buffers_persist) {
            log_file.append(persist_buffer->data(), persist_buffer->length());
        }

        // 重置后端线程使用的buffer
        buffer_spare = std::move(buffers_persist[0]);
        buffer_spare->clear();
        buffers_persist.clear();

        // 持久化到磁盘
        log_file.flush();

        if (!started_) {
            std::lock_guard<std::mutex> lg(mutex_);
            if (buffer_writing_->length() == 0) {
                running_ = false;
            }
        }
    }
    log_file.flush();
}
