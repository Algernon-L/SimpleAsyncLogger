#include "mylogger/Logger.h"
#include "mylogger/StdoutAppender.h"
using namespace std;

int main(){
    
    // 日志配置
    LogConfig::getInstance().log_buffer_nums = 2;
    LogConfig::getInstance().log_buffer_size = 64 * 1024;
    LogConfig::getInstance().log_flush_file_size = 64 * 1024 *1024;
    LogConfig::getInstance().log_flush_interval = 3;
    LogConfig::getInstance().log_level = LogLevel::INFO;

    LogConfig::getInstance().addAppender(
    "appendfile", LogAppenderInterface::Ptr(new AsyncFileAppender("./logappend/", FileWriterType::APPENDFILE)));
    
    LogConfig::getInstance().addAppender(
    "mmapfile", LogAppenderInterface::Ptr(new AsyncFileAppender("./logmmap/",FileWriterType::MMAPFILE))); 
    
    LogConfig::getInstance().addAppender(
    "stdout", LogAppenderInterface::Ptr(new StdoutAppender())); 
    
    for(int i = 0; i < 10000; i++)
        LOG_INFO << "TEST";
    
    return 0;
}