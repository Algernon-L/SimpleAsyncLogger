#pragma once
#include <mutex>
#include <condition_variable>

class CountDownLatch
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_;
};