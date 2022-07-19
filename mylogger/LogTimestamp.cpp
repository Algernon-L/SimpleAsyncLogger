#include "LogTimestamp.h"

#include <time.h>

#include <iostream>
LogTimestamp LogTimestamp::now() {
    struct timeval tv;
    if (gettimeofday(&tv, nullptr)) {
        return LogTimestamp();
    }
    return LogTimestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

time_t LogTimestamp::getSec() const {
    return microseconds_from_epoch_ / kMicroSecondsPerSecond;
}

suseconds_t LogTimestamp::getUsec() const {
    return microseconds_from_epoch_ % kMicroSecondsPerSecond;
}

std::string LogTimestamp::nowStrTime() {
    time_t timep;
    time(&timep);
    char res[64];
    strftime(res, sizeof(res), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return res;
}