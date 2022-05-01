//
// mr_environment.h
//

#ifndef MR_ENVIRONMENT_H
#define MR_ENVIRONMENT_H

#include <string>

#include "mr_string.h"

namespace mr {

class Environment {

public:
  static inline bool isEnvValue(const std::string &name,
      const std::string &value) {
    return envValue(name) == value;
  }

  static inline std::string envValue(const std::string &name) {
    char *value { std::getenv(name.c_str()) };

    if (value == nullptr || strlen(value) == 0) {
      return "";
    }
    return mr::String::lowercase(mr::String::trim(value));
  }

private:
  Environment() = delete;

};

} // ns mr

#endif /* MR_ENVIRONMENT_H */
