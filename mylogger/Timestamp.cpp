#include "Timestamp.h"

#include <time.h>

#include <iostream>
Timestamp Timestamp::now() {
    struct timeval tv;
    if (gettimeofday(&tv, nullptr)) {
        return Timestamp();
    }
    return Timestamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

time_t Timestamp::getSec() const {
    return microseconds_from_epoch_ / kMicroSecondsPerSecond;
}

suseconds_t Timestamp::getUsec() const {
    return microseconds_from_epoch_ % kMicroSecondsPerSecond;
}

std::string Timestamp::nowStrTime() {
    time_t timep;
    time(&timep);
    char res[64];
    strftime(res, sizeof(res), "%Y-%m-%d %H:%M:%S", localtime(&timep));
    return res;
}