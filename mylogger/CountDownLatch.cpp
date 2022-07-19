#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count):
    mutex_(),
    cv_(),
    count_(count)
{
}

void CountDownLatch::wait(){
    std::unique_lock<std::mutex> unilock(mutex_);
    cv_.wait(unilock, [this](){return count_ == 0;});
}

void CountDownLatch::countDown(){
    std::lock_guard<std::mutex> lg(mutex_);
    --count_;
    if(count_ == 0){
        cv_.notify_one();
    }
}