//
// mr_string.h
//

#ifndef MR_STRING_H
#define MR_STRING_H

#include <algorithm>
#include <cstring>
#include <string>

namespace mr {

class String {

public:
  static inline std::string getEndl() {
    std::ostringstream os;
    os << std::endl;
    return os.str();
  }

  static inline const std::string& newline() {
    static std::string el(mr::String::getEndl());
    return el;
  }

  static inline std::string ltrim(const std::string &original) {
    std::string copy(original);

    copy.erase(copy.begin(), std::find_if(copy.begin(), copy.end(), [](char c) {
      return !std::isspace(c);
    }));

    return copy;
  }

  static inline std::string rtrim(const std::string &original) {
    std::string copy(original);

    copy.erase(std::find_if(copy.rbegin(), copy.rend(), [](char c) {
      return !std::isspace(c);
    }).base(), copy.end());

    return copy;
  }

  static inline std::string trim(const std::string &s) {
    return mr::String::ltrim(mr::String::rtrim(s));
  }

  static inline std::string lowercase(const std::string &original) {
    std::string copy(original);

    std::transform(copy.begin(), copy.end(), copy.begin(), ::tolower);

    return copy;
  }

  static inline void lowercase(std::string &original) {
    std::transform(original.begin(), original.end(), original.begin(),
        ::tolower);
  }

private:
  String() = delete;

};

} // ns mr

#endif /* MR_STRING_H */
