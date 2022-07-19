#include "mylogger/AsyncFileAppender.h"
#include "mylogger/Logger.h"
#include "mylogger/LogAppenderInterface.h"
#include "mylogger/LogConfig.h"
#include "mylogger/FileWriterType.h"
#include "mylogger/LogStream.h"
#include <chrono>
#include <iostream>

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

int main(int argc, char **argv) {
    std::cout << "LogConfig：" << "\n";
    std::cout << "log_flush_file_size:" << LogConfig::getInstance().log_flush_file_size << "\n";
    std::cout << "log_flush_interval:" << LogConfig::getInstance().log_flush_interval << "\n";
    std::cout << "log_buffer_size:" << LogConfig::getInstance().log_buffer_size << "\n";
    std::cout << "log_buffer_nums:"<< LogConfig::getInstance().log_buffer_nums << "\n";

    // 加入一个输出目的地
    LogConfig::getInstance().addAppender(
        "asyncfile", LogAppenderInterface::Ptr(new AsyncFileAppender("./log/",FileWriterType::APPENDFILE)));   
    // 计时
    const int count = 1000000;
    {
        Timer timer;
        
        for (int index = 0; index < count; ++index){
            LOG_INFO << "helloworld!" << " index" << index;
        }
        std::cout << count << "次日志输出 ";
    }
}
