//
//  mr_log.cpp
//

#include <sstream>

#include "mr_log.h"
#include "mr_time.h"
#include "mr_environment.h"

namespace mr {

std::string Logging::ts() const {
  return mr::Time::ts();
}

LoggerProxy<LogError> error;
LoggerProxy<LogWarn> warn;
LoggerProxy<LogInfo> info;
LoggerProxy<LogDebug> debug;
LoggerProxy<LogTrace> trace;

namespace logger {
namespace id {
std::atomic<unsigned long long> counter(0);

const std::string& threadId() {
  static const std::string prefix("thread-");

  static std::atomic<int> count { 1 };

  const thread_local std::string name(
      prefix + std::to_string(count.fetch_add(1)));

  return name;
}

} // ns id
} // ns logger

template<typename T>
bool Logger<T>::isNoLevelOn() {
  static const bool isLevel(
      static_cast<int>(mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_TRACE))
      + static_cast<int>(mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_DEBUG))
      + static_cast<int>(mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_WARN))
      + static_cast<int>(mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_INFO))
      + static_cast<int>(mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_ERROR)) == 0);

  return isLevel;
}

//

template<>
const char* Logger<LogError>::getPrefix() const {
  return _LOGLEVEL_ERROR;
}

template<>
const char* Logger<LogWarn>::getPrefix() const {
  return _LOGLEVEL_WARN;
}

template<>
const char* Logger<LogInfo>::getPrefix() const {
  return _LOGLEVEL_INFO;
}

template<>
const char* Logger<LogDebug>::getPrefix() const {
  return _LOGLEVEL_DEBUG;
}

template<>
const char* Logger<LogTrace>::getPrefix() const {
  return _LOGLEVEL_TRACE;
}

//

template<>
bool Logger<LogTrace>::isLevelOn() {
  static const bool isLevel(
      mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_TRACE));

  return isLevel;
}

template<>
bool Logger<LogDebug>::isLevelOn() {
  static const bool isLevel(			    
      mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_DEBUG) 
      || mr::Logger < mr::LogTrace > ::isLevelOn());

  return isLevel;
}

template<>
bool Logger<LogInfo>::isLevelOn() {
  static const bool isLevel(
      isNoLevelOn() ?
          true :
          (mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_INFO)
              || mr::Logger < mr::LogDebug > ::isLevelOn()));

  return isLevel;
}

template<>
bool Logger<LogWarn>::isLevelOn() {
  static const bool isLevel(
      isNoLevelOn() ?
          true :
          (mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_WARN)
              || mr::Logger < mr::LogInfo > ::isLevelOn()));

  return isLevel;
}

template<>
bool Logger<LogError>::isLevelOn() {
  static const bool isLevel(
      isNoLevelOn() ?
          true :
          (mr::Environment::isEnvValue(_LOGLEVEL, _LOGLEVEL_ERROR)
              || mr::Logger < mr::LogWarn > ::isLevelOn()));

  return isLevel;
}

} // ns mr

