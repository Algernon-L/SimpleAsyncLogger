#pragma once

#include <stdint.h>
#include <string>
#include <sys/time.h>

class LogTimestamp {
public:
  static constexpr uint64_t kMicroSecondsPerSecond = 1000 * 1000;

  LogTimestamp() : microseconds_from_epoch_(0) {}

  explicit LogTimestamp(uint64_t microseconds_from_epoch)
      : microseconds_from_epoch_(microseconds_from_epoch) {}

  uint64_t getMicroSecondsFromEpoch() const { return microseconds_from_epoch_; }
  time_t getSec() const;
  suseconds_t getUsec() const;
  static std::string nowStrTime();
  static LogTimestamp now();

private:
  uint64_t microseconds_from_epoch_;
};

inline bool operator<(const LogTimestamp &lhs, const LogTimestamp &rhs) {
  return lhs.getMicroSecondsFromEpoch() < rhs.getMicroSecondsFromEpoch();
}

inline bool operator==(const LogTimestamp &lhs, const LogTimestamp &rhs) {
  return lhs.getMicroSecondsFromEpoch() == rhs.getMicroSecondsFromEpoch();
}

inline LogTimestamp operator+(const LogTimestamp &lhs, uint64_t micro_seconds) {
  return LogTimestamp(lhs.getMicroSecondsFromEpoch() + micro_seconds);
}

inline int64_t operator-(const LogTimestamp &lhs, const LogTimestamp &rhs) {
  return lhs.getMicroSecondsFromEpoch() - rhs.getMicroSecondsFromEpoch();
}