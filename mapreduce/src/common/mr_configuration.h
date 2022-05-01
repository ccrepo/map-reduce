//
// mr_configurartion.h
//

#ifndef MR_CONFIGURATION_H
#define MR_CONFIGURATION_H

#include <cstdint>

namespace mr {

class Configuration {
public:

  // future arguments - TODO move to command line arguments with defaults
  static constexpr const uint32_t _DEFAULT_COORDINATOR_THREAD_COUNT { 200 };
  static constexpr const char *   _DEFAULT_MATCH_STRING { "and" };
  static constexpr const uint32_t _DEFAULT_MATCH_WRITE_BATCHSIZE  { 250 };
  static constexpr const uint32_t _DEFAULT_MUTEX_RETRYLIMIT { 10 };
  static constexpr const char *   _DEFAULT_FILELOCK_FILENAME  { "/tmp/mapreduce.lock" };
  static constexpr const uint32_t _DEFAULT_FILELOCK_RETRYLIMIT { 10 };
  static constexpr const bool     _DEFAULT_DUMP_RESULTS_FLAG { true };
  
private:
  Configuration() = delete;
};

}

#endif /* MR_CONFIGURATION_H */
