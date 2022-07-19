#include "mylogger/AsyncFileAppender.h"
#include "mylogger/Logger.h"
#include "mylogger/LogAppenderInterface.h"
#include "mylogger/LogConfig.h"
#include "mylogger/FileWriterType.h"
#include "mylogger/LogStream.h"
#include <chrono>
#include <iostream>
#include <vector>

class Timer{
public:
    Timer(){
        start = std::chrono::system_clock::now();
    }
    ~Timer(){
        auto end = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "花费时间：" << duration.count() << "us\n";
    }
private:
    std::chrono::system_clock::time_point start;
};

void worker(int start, int end){
    Timer timer;
    for (int index = start; index < end; ++index){
            LOG_INFO << "helloworld!" << " index" << index;
    }
}

int main(int argc, char **argv) {
    LogConfig::getInstance().log_flush_file_size = 64 * 1024 * 1024;
    std::cout << "LogConfig：" << "\n";
    std::cout << "log_flush_file_size:" << LogConfig::getInstance().log_flush_file_size << "\n";
    std::cout << "log_flush_interval:" << LogConfig::getInstance().log_flush_interval << "\n";
    std::cout << "log_buffer_size:" << LogConfig::getInstance().log_buffer_size << "\n";
    std::cout << "log_buffer_nums:"<< LogConfig::getInstance().log_buffer_nums << "\n";

    // 加入一个输出目的地
    LogConfig::getInstance().addAppender(
        "asyncfile", LogAppenderInterface::Ptr(new AsyncFileAppender("./log/",FileWriterType::MMAPFILE)));   
    
    std::vector<std::thread> th;
    th.reserve(10);
    for(int i = 1; i <= 10; i++){
        th.emplace_back(std::thread(worker, (i - 1) * 10000 + 1, i * 10000));
    }
    for(int i = 0; i < 10; i++){
        th[i].join();
    }
}
