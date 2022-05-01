//
// mr_signal.h
//

#ifndef MR_SIGNAL_H
#define MR_SIGNAL_H

#include <string>
#include <cstring>
#include <iostream>

#include <signal.h>

#include "mr_global.h"
#include "mr_time.h"

namespace mr {

class SignalInit;

class Signal {

public:
  static inline void signal_handler(int signal_num) {
    static unsigned long long startTimeMS(mr::Time::ts_EPOCH_MS());

    std::cerr << "stopping[" << signal_num << "] ..." << std::endl;

    unsigned long long nowMS(mr::Time::ts_EPOCH_MS());

    if (nowMS - startTimeMS > _SIGNAL_EXIT_FORCE_TIMEOUT) {
      exit(1);
    }

    mr::Global::setExit();
  }

  class SignalInit {
  public:
    SignalInit() {
      static Signal g; // set up  exiter
    }

    ~SignalInit() {}

  private:
    SignalInit(const SignalInit&) = delete;
    SignalInit& operator=(const SignalInit&) = delete;
  };

private:
  Signal() { // might have to make this laxy ... 
    std::cerr << "." << std::endl;
    signal(SIGKILL, Signal::signal_handler);
    signal(SIGTERM, Signal::signal_handler);
    signal(SIGINT, Signal::signal_handler);
  }

  static constexpr uint32_t _SIGNAL_EXIT_FORCE_TIMEOUT { 2000 };

private:
  Signal(const Signal&) = delete;
  Signal& operator=(const Signal&) = delete;

  friend class mr::SignalInit;
};

// wouldn't do this in real life due to static race conditions 
// but done like this to show how it could be done.
// could make it lazy (but early!) to get around that
extern Signal::SignalInit __SignalInit;

} // ns mr

#endif /* MR_SIGNAL_H */
