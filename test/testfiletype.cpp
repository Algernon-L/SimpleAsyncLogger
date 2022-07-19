#include "mylogger/Logger.h"
#include "mylogger/StdoutAppender.h"
using namespace std;

int main(){
    
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