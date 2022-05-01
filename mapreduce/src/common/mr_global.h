//
// mr_global.h
//

#ifndef MR_GLOBAL_H
#define MR_GLOBAL_H

#include <atomic>

namespace mr {

class Global {

public:
  inline static bool isExit() {
    return _exit.load();
  }
  
  inline static void setExit() {
    _exit.store(true);
  }

private:
  static std::atomic<bool> _exit;

private:
  Global() = delete;
};

} // ns mr

#endif /* MR_SIGNAL_H */
