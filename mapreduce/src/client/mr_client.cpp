//
// mr_client.cpp
//

#include <atomic>
#include <csignal>
#include <cstring>
#include <sstream>

#include "mr_sleep.h"
#include "mr_global.h"
#include "mr_OS.h"
#include "mr_datafile.h"
#include "mr_filelock.h"
#include "mr_log.h"
#include "mr_memory.h"
#include "mr_time.h"
#include "mr_random.h"
#include "mr_timer.h"

// TODO 
// 1. expand command line arguments. add const vars.
// 2. add cond var waits or timeout waits
// 3. put atomic lock inside shared memory, use for memory locking
// 4. documentation
// 5  int64_t vs unsigned for the result + shrink int sizes
// 6. use filelocker and arg to sync startup
// 7. ensure ordered exit - shared memory corruption
// 8. reorg build re. defines fo the two main builds. mr_memory is the biggest problem here.

int main(int argc, const char *argv[]) {

  LOGGER_INFO("starting..");

  if (argc != 2 || !argv[1] || !strlen(argv[1])) {
    LOGGER_ERROR(
        "incorrect arguments - run with single argument of data filename");
    return 1;
  }

  std::shared_ptr<long long> ms(std::make_shared<long long>(0));

  std::shared_ptr < mr::Memory
      > memory(std::make_shared < mr::Memory > (mr::_MEMORYLAYOUT_NAME));

  if (!memory->init()) {
    LOGGER_ERROR("could not init shared memory");
    return 1;
  }

  mr::Datafile datafile(argv[1]);

  {
    mr::MemoryRAIIUnlockerWrapper autoUnlocker(memory);

    if (!autoUnlocker.lockMemory()) {
      LOGGER_ERROR("could not lock memory mutex");
      return 1;
    } else {
      LOGGER_DEBUG("locked memory mutex");
    }

    while (!mr::Global::isExit() && !datafile.eof()) {
      if (!memory->writeLine(datafile.nextline())) {
        LOGGER_ERROR("shared memory write failed");
        return 1;;
      }
    }

    LOGGER_INFO("loaded ", datafile.linesLoaded(), " rows");
  }

  std::vector<int64_t> results;

  {
    mr::Timer timer(ms);

    results.reserve(datafile.linesLoaded());

    while (!mr::Global::isExit()) {

      {
        mr::MemoryRAIIUnlockerWrapper autoUnlocker(memory);

        if (autoUnlocker.lockMemory()) {
          LOGGER_TRACE("got lock");

          if (memory->isFinished()) {
            LOGGER_INFO("coordinator is finished"); 
            memory->load(results);
            break;
          } else {
            LOGGER_TRACE("not finished");
          }
        } else {
          LOGGER_TRACE("could not get lock");
        }
      }

      mr::Sleep::sleepMiniscule();
    }
  }

  if (!mr::Global::isExit()) {
    LOGGER_INFO("matched: ", results.size(), " total");

    if (mr::Configuration::_DEFAULT_DUMP_RESULTS_FLAG &&
        mr::Logger<mr::LogInfo>::isLevelOn()) {
      
      std::ostringstream os;
      
      for (auto result : results) {
        os << result << " ";
      }
      
      LOGGER_INFO("dump: { ", os.str(), " }");
    }

    LOGGER_INFO("solution time: ", *ms, " ms");
  }

  LOGGER_INFO("fini.");

  return 0;
}
