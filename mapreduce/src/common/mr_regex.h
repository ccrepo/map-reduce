//
// mr_regex.h
//

#ifndef MR_REGEX_H
#define MR_REGEX_H

#include <tuple>
#include <thread>
#include <atomic>
#include <memory>
#include <atomic>

#include "mr_pipe.h"

namespace mr {

class Memory;

class Regex {

public:
  Regex(const std::string &searchString,
      const std::tuple<uint64_t, uint64_t> &searchRange,
      const std::shared_ptr<mr::Memory> &memory, uint32_t batchSize, uint32_t reserveSize);

  ~Regex();

  bool start();

  bool stop();

  inline bool finished() const {
    return _finished;
  }

  inline bool empty() const {
    return this->_pipe->empty();
  }

  inline void pop(std::vector<int64_t> &elements) {
    this->_pipe->pop(elements);
  }

private:
  std::shared_ptr<std::thread> _thread;
  std::shared_ptr<mr::Pipe> _pipe;

  std::atomic<bool> _exit;
  std::atomic<bool> _finished;

  const std::shared_ptr<mr::Memory> _memory;

  const std::tuple<uint64_t, uint64_t> _searchRange;
  const std::string _searchString;
  const uint32_t _batchSize;
  const uint32_t _reserveSize;

private:
  static void process(Regex *regex);

  inline std::shared_ptr<mr::Pipe> pipe() const {
    return this->_pipe;
  }

  inline const std::shared_ptr<mr::Memory>& memory() const {
    return this->_memory;
  }

  inline std::tuple<uint64_t, uint64_t> searchRange() const {
    return this->_searchRange;
  }

  inline const std::string& searchString() const {
    return this->_searchString;
  }

  inline uint32_t batchSize() const {
    return this->_batchSize;
  }

  inline uint32_t reserveSize() const {
    return this->_reserveSize;
  }
  
private:
  Regex(const Regex&) = delete;
  Regex& operator=(const Regex&) = delete;

};

} // ns mr

#endif // ns MR_REGEX_H 

