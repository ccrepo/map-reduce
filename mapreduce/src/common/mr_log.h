//
// mr_log.h
//

#ifndef MR_LOG_H
#define MR_LOG_H

#include <iostream>
#include <sstream>
#include <atomic>

#include "mr_OS.h"
#include "mr_string.h"

namespace mr {

namespace logger {
namespace id {
extern std::atomic<unsigned long long> counter;

const std::string& threadId();
} // ns id
} // ns logger

class LogError;
class LogWarn;
class LogInfo;
class LogDebug;
class LogTrace;

template<typename T>
class LoggerProxy;

class Logging {

public:
  static constexpr const char *_LOGLEVEL { "MR_LOGLEVEL" };
  static constexpr const char *_LOGLEVEL_TRACE { "trace" };
  static constexpr const char *_LOGLEVEL_DEBUG { "debug" };
  static constexpr const char *_LOGLEVEL_INFO  { "info" };
  static constexpr const char *_LOGLEVEL_WARN  { "warn" };
  static constexpr const char *_LOGLEVEL_ERROR { "error" };

protected:
  std::string ts() const;
};

template<typename T>
class Logger: public Logging {

private:
  Logger() :
      _id(mr::logger::id::counter.fetch_add(1)) {
  }

  friend class LoggerProxy<T> ;

public:
  virtual ~Logger() {
    this->flush();
  }

  Logger(const Logger &logger) :
      _id(mr::logger::id::counter.fetch_add(1)) {
    this->_os << logger._os.str();
  }

  template<typename U>
  Logger<T>& operator<<(const U &u) {
    static bool isLevel(Logger<T>::isLevelOn());

    if (isLevel) {
      this->_os << u;
    }

    return (*this);
  }

  Logger<T>& operator<<(Logger<T>& (*fp)(Logger<T> &logger)) {
    return fp(*this);
  }

  void flush() {
    if (this->_os.str().length() > 0) {
      std::ostringstream os;

      os << this->ts() << " [" << mr::logger::id::threadId() << "|"
          // << _id
          // << "|"
          // << std::this_thread::get_id()
          // << "|"
          //<< (static_cast<void*>(this)) 
          //<< "|"
          << this->getPrefix() << "] "
          << this->_os.str();

      std::cout << os.str();

      this->clear();
    }
  }

  void clear() {
    this->_os.clear();
    this->_os.str("");
  }

  static bool isLevelOn();

  static bool isNoLevelOn();

  static std::string getTimestamp();

  const char* getPrefix() const;

private:
  Logger& operator=(const Logger&) = delete;

private:
  std::ostringstream _os;

  const long long _id;
};

template<typename T>
class LoggerProxy {

public:
  LoggerProxy() {
  }

  virtual ~LoggerProxy() {
    std::cout.flush();
  }

  template<typename U>
  Logger<T> operator<<(const U &data) {
    Logger<T> logger;

    logger << data;

    return logger;
  }

private:
  LoggerProxy(const LoggerProxy&) = delete;
  LoggerProxy& operator=(const LoggerProxy&) = delete;
};

extern LoggerProxy<LogError> error;
extern LoggerProxy<LogWarn> warn;
extern LoggerProxy<LogInfo> info;
extern LoggerProxy<LogDebug> debug;
extern LoggerProxy<LogTrace> trace;

template<typename T>
Logger<T>& endl(Logger<T> &logger) {
  static const std::string newline(mr::String::newline());
  logger << newline;
  return logger;
}

class LogError;
class LogWarn;
class LogInfo;
class LogDebug;
class LogTrace;

//

inline std::ostringstream& operator<<(std::ostringstream &os,
    const std::ostringstream &s) {
  os << s.str();
  return os;
}

template<typename T>
void log(std::ostringstream &os, const T &t) {
  os << t;
}

template<typename T, typename ... Args>
void log(std::ostringstream &os, const T &t, const Args &... args) {
  os << t;
  mr::log<Args...>(os, args...);
}

template<typename T, typename ... Args>
void log(mr::LoggerProxy<T> &logger, const char *file, uint32_t lineno,
    const char *function, const Args &... args) {
  std::ostringstream os;

  os << file << ":" << lineno << "(" << function << ") ";

  mr::log<Args...>(os, args...);

  logger << os.str() << mr::endl;
}

#define LOGGER_TRACE(...) {                                                \
if (mr::Logger<mr::LogTrace>::isLevelOn())                                 \
{                                                                          \
constexpr const char* basename(mr::OS::basename(__FILE__));                \
mr::log<mr::LogTrace>                                                      \
(mr::trace, basename,                                                      \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOGGER_DEBUG(...) {                                                \
if (mr::Logger<mr::LogDebug>::isLevelOn())                                 \
{                                                                          \
constexpr const char* basename(mr::OS::basename(__FILE__));                \
mr::log<mr::LogDebug>                                                      \
(mr::debug, basename,                                                      \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOGGER_INFO(...) {                                                 \
if (mr::Logger<mr::LogInfo>::isLevelOn())                                  \
{                                                                          \
constexpr const char* basename(mr::OS::basename(__FILE__));                \
mr::log<mr::LogInfo>                                                       \
(mr::info, basename,                                                       \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOGGER_WARN(...) {                                                 \
if (mr::Logger<mr::LogWarn>::isLevelOn())                                  \
{                                                                          \
constexpr const char* basename(mr::OS::basename(__FILE__));                \
mr::log<mr::LogWarn>                                                       \
(mr::warn, basename,                                                       \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

#define LOGGER_ERROR(...) {                                                \
if (mr::Logger<mr::LogError>::isLevelOn())                                 \
{                                                                          \
constexpr const char* basename(mr::OS::basename(__FILE__));                \
mr::log<mr::LogError>                                                      \
(mr::error, basename,                                                      \
__LINE__, __FUNCTION__, __VA_ARGS__);                                      \
}                                                                          \
}

} // ns mr

#endif /* MR_LOG_H */

