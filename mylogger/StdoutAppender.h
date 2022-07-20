#pragma once
#include "LogAppenderInterface.h"
#include <iostream>

class StdoutAppender :public LogAppenderInterface{
public:
  StdoutAppender() = default;
  ~StdoutAppender() = default;
  void append(const char *msg, size_t len);
};

void StdoutAppender::append(const char *msg, size_t len){
  std::cout << msg << "\n";
}