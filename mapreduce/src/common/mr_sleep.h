//
// mr_sleep.h
//

#ifndef MR_SLEEP_H
#define MR_SLEEP_H

#include <chrono>
#include <thread>

#include "mr_random.h"

namespace mr {

class Sleep {

public:
  static constexpr const uint32_t _DURATION_MINISCULE { 15 };
  static constexpr const uint32_t _DURATION_SHORTEST { 30 };
  static constexpr const uint32_t _DURATION_SHORTER { 100 };
  static constexpr const uint32_t _DURATION_SHORT { 250 };
  static constexpr const uint32_t _DURATION_MEDIUM { 500 };
  static constexpr const uint32_t _DURATION_LONG { 750 };
  static constexpr const uint32_t _DURATION_LONGER { 1000 };
  static constexpr const uint32_t _DURATION_LONGEST { 2000 };

public:
  // fixed
  inline static void sleepMiniscule() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_MINISCULE));
  }

  inline static void sleepShortest() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_SHORTEST));
  }

  inline static void sleepShorter() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_SHORTER));
  }

  inline static void sleepShort() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_SHORT));
  }

  inline static void sleepMedium() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_MEDIUM));
  }

  inline static void sleepLong() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_LONG));
  }

  inline static void sleepLonger() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_LONGER));
  }

  inline static void sleepLongest() {
    std::this_thread::sleep_for(std::chrono::milliseconds(_DURATION_LONGEST));
  }

  // general purpose
  inline static void sleep(uint32_t ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
  }

  inline static void sleepRandomInRange(uint32_t lower_ms, uint32_t upper_ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(mr::Random::randomInRange(lower_ms, upper_ms)));
  }

private:
  Sleep() = delete;

};

} // ns mr

#endif /* MR_SLEEP_H */
