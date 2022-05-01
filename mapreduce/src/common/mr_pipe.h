//
// mr_pipe.h
//

#ifndef MR_PIPE_H
#define MR_PIPE_H

#include <atomic>
#include <vector>
#include <string>
#include <queue>
#include <mutex>

#include "mr_sleep.h"

namespace mr {

class Pipe {

public:
  Pipe(uint32_t reserveSize) :
      _elements(), _mutex() {
    _elements.reserve(reserveSize); 
  }

  ~Pipe() {
  }

  void push(std::vector<int64_t> &elements) {
    std::unique_lock < std::timed_mutex > lock(_mutex, std::defer_lock);

    if (lock.try_lock_for(
        std::chrono::milliseconds(
            mr::Random::randomInRange(mr::Sleep::_DURATION_SHORTEST,
                mr::Sleep::_DURATION_SHORTER)))) {

      std::move(elements.begin(), elements.end(),
          std::back_inserter(_elements));
      elements.erase(elements.begin(), elements.end());

      _empty = _elements.empty();
    }
  }

  void pop(std::vector<int64_t> &elements) {
    std::unique_lock < std::timed_mutex > lock(_mutex, std::defer_lock);
    if (lock.try_lock_for(
        std::chrono::milliseconds(
            mr::Random::randomInRange(mr::Sleep::_DURATION_SHORTEST,
                mr::Sleep::_DURATION_SHORTER)))) {

      std::move(_elements.begin(), _elements.end(),
          std::back_inserter(elements));
      _elements.erase(_elements.begin(), _elements.end());

      _empty = _elements.empty();
    }
  }

  bool empty() const {
    return _empty;
  }

private:
  std::vector<int64_t> _elements;
  std::timed_mutex _mutex;
  std::atomic<bool> _empty;

private:
  Pipe(const Pipe&) = delete;
  Pipe& operator=(const Pipe&) = delete;

};

} // ns mr

#endif /* MR_PIPE_H */
