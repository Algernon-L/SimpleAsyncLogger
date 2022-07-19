#pragma once
#include <sys/types.h>
class LogBuffer {
public:
    LogBuffer(size_t total = 1024 * 1024 * 10);// 初始化10MB
    ~LogBuffer();

    void clear();// 清空缓冲区
    void append(const char *data, size_t len);// 填入日志
    const char *data() const;// get数据
    size_t length() const;// 数据长度
    size_t available() const;// 缓冲区可用长度
    // for used by GDB
    const char *debugString();
    void setCookie(void (*cookie)()) { cookie_ = cookie; }// 哨兵

private:
    char *data_;
    const size_t total_;
    size_t available_;
    size_t cur_;
    // Must be outline function for cookies.
    static void cookieStart();
    static void cookieEnd();

    void (*cookie_)();
};