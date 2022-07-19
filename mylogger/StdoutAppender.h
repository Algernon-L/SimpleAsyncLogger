#include "LogAppenderInterface.h"
#include <iostream>

class StdoutAppender :public LogAppenderInterface{
public:
  StdoutAppender();
  ~StdoutAppender() {}
  void append(const char *msg, size_t len);
};

StdoutAppender::StdoutAppender(){}

void StdoutAppender::append(const char *msg, size_t len){
  std::cout << msg << "\n";
}