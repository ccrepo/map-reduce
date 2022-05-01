//
// mr_regex.cpp
//

#include <thread>
#include <regex>

#include "mr_regex.h"
#include "mr_log.h"
#include "mr_memory.h"
#include "mr_sleep.h"

namespace mr {

Regex::Regex(const std::string &searchString,
    const std::tuple<uint64_t, uint64_t> &searchRange,
    const std::shared_ptr<mr::Memory> &memory, uint32_t batchSize, uint32_t reserveSize) :
    _thread(), _pipe(std::make_shared<mr::Pipe>(reserveSize)), _exit(false), _finished(), _memory(memory), _searchRange(
        searchRange), _searchString(searchString), _batchSize(
        batchSize), _reserveSize(reserveSize) {
  LOGGER_INFO("regex range ", std::get < 0 > (_searchRange),
      ":", std::get < 1 > (_searchRange));
  this->_finished.store(false);
  this->start();
}

Regex::~Regex() {
  LOGGER_DEBUG("destroying regex");
  this->stop();
  LOGGER_DEBUG("destroyed regex");
}

bool Regex::start() {
  LOGGER_DEBUG("starting thread");

  if (_thread.get()) {
    LOGGER_WARN("regex thread already started");
    return true;
  }

  this->_thread.reset(new std::thread(mr::Regex::process, this));
  LOGGER_DEBUG("started thread");

  return true;
}

bool Regex::stop() {
  LOGGER_DEBUG("stopping regex");

  this->_exit.store(true);

  if (!_thread.get()) {
    return true;
  }

  this->_thread->join();

  this->_thread.reset(static_cast<std::thread*>(0));

  LOGGER_DEBUG("stopped thread");
  return true;
}

void Regex::process(Regex *regex) {
  LOGGER_DEBUG("started regex thread");

  std::shared_ptr < mr::Memory > memory(regex->memory());
  mr::MemoryLayout *memorylayout(memory->memoryLayout());
  if (!memorylayout) {
    LOGGER_INFO("shared memorylayout is null");
    return;
  }

  std::tuple < uint64_t, uint64_t > searchRange(regex->searchRange());
  std::shared_ptr < mr::Pipe > pipe(regex->pipe());
  std::regex re(regex->searchString(),
      std::regex_constants::ECMAScript | std::regex_constants::icase);

  std::vector < int64_t > results;
  results.reserve(regex->reserveSize());

  LOGGER_DEBUG("processing range ", std::get < 0 > (searchRange), ":",
      std::get < 1 > (searchRange));

  for (uint64_t i(std::get < 0 > (searchRange));
       i < std::get < 1 > (searchRange) && !mr::Global::isExit(); i++) {
    LOGGER_TRACE("checking position index ", i);
    
    if (std::regex_search(&memorylayout->_lines[memorylayout->_offsets[i]], re)) {
      LOGGER_TRACE("matched line at position index ", i);
      results.push_back(i);
    } else {
      LOGGER_TRACE("failed line at position index ", i);
    }

    if (!(i % regex->batchSize()) && !results.empty()) {
      LOGGER_TRACE("pushing ", results.size()," elements to pipe");
      pipe->push(results);
      mr::Sleep::sleepShort();
    }
  }

  while (!results.empty() && !mr::Global::isExit()) {
    LOGGER_TRACE("pushing ", results.size()," to pipe");
    pipe->push(results);
    mr::Sleep::sleepShort();
  }

  regex->_finished.store(true);

  if (mr::Global::isExit()) {
    LOGGER_DEBUG("regex shutting down");
  } else {
    LOGGER_DEBUG("finished regex process thread");
  }
}

} // ns mr

