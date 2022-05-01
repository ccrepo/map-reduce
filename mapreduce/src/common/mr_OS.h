//
// mr_os.h
//

#ifndef _MR_OS_H
#define _MR_OS_H

namespace mr {

#ifdef _MR_CLIENT_BUILD
#elif _MR_COORDINATOR_BUILD
#else
static_assert(0, "_MR_CLIENT_BUILD or _MR_COORDINATOR must be defined");
#endif

class OS {

public:
  static constexpr char path_separator { '/' };

  static constexpr const char* str_end(const char *s) {
    return *s ? str_end(s + 1) : s;
  }

  static constexpr bool str_slant(const char *s) {
    return *s == path_separator ? true : (*s ? str_slant(s + 1) : false);
  }

  static constexpr const char* r_slant(const char *s) {
    return *s == path_separator ? (s + 1) : r_slant(s - 1);
  }

  static constexpr const char* basename(const char *s) {
    return str_slant(s) ? r_slant(str_end(s)) : s;
  }

private:
  OS() = delete;
};

} // ns mr

#endif /* _MR_OS_H */
