#include "LogConfig.h"

// 注册删除目的地
void LogConfig::addAppender(const std::string &appender_name,
                LogAppenderInterface::Ptr appender)
{   
    std::lock_guard<std::mutex> lg(mtx);
    destinationmap[appender_name] = appender;
}

void LogConfig::delAppender(const std::string &appender_name)
{
    std::lock_guard<std::mutex> lg(mtx);
    for(auto iter = destinationmap.begin(); iter != destinationmap.end(); iter++){
        if(iter->first == appender_name){
            destinationmap.erase(iter->first);
            break;
        }
    }
}

void LogConfig::clearAppender()
{
    std::lock_guard<std::mutex> lg(mtx);
    destinationmap.clear();
}