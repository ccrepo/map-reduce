//
// mr_time.h
//

#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <chrono>
using namespace std::chrono_literals;

#ifndef MR_TIME_H
#define MR_TIME_H

namespace mr {

class Time {

public:
  inline static std::string ts() {
    auto ts { std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now()) };

    std::ostringstream os;
    os << std::put_time(std::localtime(&ts), "%H:%M:%S"); // "%Y%m%d %H:%M:%S"
        
    return os.str();
  }

  inline static long long ts_EPOCH_MS() {
    static const auto epoch(std::chrono::system_clock::from_time_t(0));

    return std::chrono::duration_cast < std::chrono::milliseconds
        > (std::chrono::system_clock::now() - epoch).count();
  }

  inline static long long ts_EPOCH_US() {
    static const auto epoch(std::chrono::system_clock::from_time_t(0));

    return std::chrono::duration_cast < std::chrono::microseconds
        > (std::chrono::system_clock::now() - epoch).count();
  }

  inline static time_t time_t_SECONDS() {
    return std::time(NULL);
  }

private:
  Time() = delete;
};

}

#endif /* MR_TIME_H */
