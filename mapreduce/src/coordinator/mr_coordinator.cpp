//
// mr_coordinator.cpp
//

#include <iostream>
#include <thread>
#include <climits>
#include <array>
#include <tuple>
#include <vector>
#include <memory>
#include <algorithm>
#include <climits>
#include <sys/file.h>
#include <signal.h>

#include "mr_signal.h"
#include "mr_OS.h"
#include "mr_configuration.h"
#include "mr_sleep.h"
#include "mr_filelock.h"
#include "mr_regex.h"
#include "mr_log.h"
#include "mr_memory.h"
#include "mr_algorithm.h"
#include "mr_timer.h"

namespace mr {
void init();
}

int main(int argc, const char *argv[]) {

  LOGGER_INFO("starting..");

  if (argc != 1) {
    LOGGER_ERROR(
        "incorrect arguments - run with single argument of data filename");
    return 1;
  }

  std::shared_ptr<long long> ms(std::make_shared<long long>(0));

  std::shared_ptr < mr::Memory
      > memory(std::make_shared < mr::Memory > (mr::_MEMORYLAYOUT_NAME));

  if (!memory->init()) {
    LOGGER_INFO("could not init shared memory");
    return 1;
  }

  std::vector < std::shared_ptr < mr::Regex >> regexs;

  std::vector<std::tuple<uint64_t, uint64_t>> searchRanges;

  std::vector < int64_t > results;

  uint32_t runningTotal(0);

  {
    mr::MemoryRAIIUnlockerWrapper autoUnlocker(memory);
    autoUnlocker.lockMemory(); // blocking

    LOGGER_DEBUG("shared memory contains ", memory->offsetCount(), " records");

    if (!mr::Algorithm::partition(memory->offsetCount(),
        mr::Configuration::_DEFAULT_COORDINATOR_THREAD_COUNT, searchRanges)) {

      LOGGER_INFO("could not partition");
      return 1;
    }
  }

  {
    mr::Timer timer(ms);

    for (const auto &searchRange : searchRanges) {
      regexs.push_back(
          std::make_shared < mr::Regex
              > (mr::Configuration::_DEFAULT_MATCH_STRING, searchRange, memory, mr::Configuration::_DEFAULT_MATCH_WRITE_BATCHSIZE, (memory->offsetCount()
                  / searchRanges.size()) + 1));
    }

    results.reserve(memory->offsetCount());

    LOGGER_INFO("created: ", regexs.size(), " regex sub-threads");

    while (!mr::Global::isExit()) {

      bool finished { std::all_of(regexs.begin(), regexs.end(),
          [](const auto &regex) {
            return regex->finished();
          }) };

      for (auto &regex : regexs) {
        if (!regex->empty()) {
          regex->pop(results);
        }
      }

      LOGGER_TRACE("sending running total ", runningTotal);

      if (finished) {
        LOGGER_INFO("all regexs finished");
        break;
      }

      if ((!mr::Global::isExit() && !results.empty())) {
        mr::Sleep::sleepMiniscule();
      }

      while (!mr::Global::isExit() && !results.empty()) {
        LOGGER_DEBUG("locking memory");

        {
          mr::MemoryRAIIUnlockerWrapper autoUnlocker(memory);

          if (autoUnlocker.lockMemory()) {
            LOGGER_DEBUG("locked memory");
            runningTotal += results.size();
            memory->writeResult(results);
          } else {
            LOGGER_DEBUG("could not lock memory");
          }
        }

        mr::Sleep::sleepShortest();
      }
    }

    while (!mr::Global::isExit() && !results.empty()) {

      {
        mr::MemoryRAIIUnlockerWrapper autoUnlocker(memory);

        if (autoUnlocker.lockMemory()) {
          runningTotal += results.size();
          memory->writeResult(results);
        }
      }

      mr::Sleep::sleepShortest();
    }
  }

  {
    mr::MemoryRAIIUnlockerWrapper autoUnlocker(memory);

    if (autoUnlocker.lockMemory()) {
      memory->writeResult(results);
    }

    memory->setFinished();
  }

  if (!mr::Global::isExit()) {
    LOGGER_INFO("matched: ", runningTotal, " total - sending to shared memory");

    LOGGER_INFO("solution time: ", *ms, " ms");
  }

  LOGGER_INFO("fini.");

  return 0;
}
