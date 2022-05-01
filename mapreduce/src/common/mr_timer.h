//
// mr_timer.h
//

#ifndef MR_TIMER_H
#define MR_TIMER_H

#include "mr_time.h"

namespace mr {

class Timer {
  
public:
  Timer(const std::shared_ptr<long long> &result) :
      _result(result), _start(mr::Time::ts_EPOCH_MS()) {
  }

  virtual ~Timer() {
    if (std::shared_ptr<long long> result = _result.lock()) {
      (*result) = mr::Time::ts_EPOCH_MS() - this->_start;
    }
  }

private:
  std::weak_ptr<long long> _result;
  const long long _start;

private:
  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;

};

} // ns mr

#endif // ns MR_TIMER_H