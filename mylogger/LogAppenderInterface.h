#pragma once
#include <memory>

// 接口，供用户选择传输写入目的地
class LogAppenderInterface {
public:
  using Ptr = std::shared_ptr<LogAppenderInterface>;

public:
  virtual ~LogAppenderInterface() {}
  virtual void append(const char *msg, size_t len) = 0;
};