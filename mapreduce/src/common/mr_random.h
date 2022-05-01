//
// mr_random.h
//

#ifndef MR_RANDOM_H
#define MR_RANDOM_H

#include <random>

namespace mr {

class Random {

public:
  inline static uint32_t randomInRange(uint32_t lower, uint32_t upper) {
    if (upper <= lower) {
      return lower; // silent fail ... could throw assert instead
    }
    
    static std::random_device device;
    static std::mt19937 generator(device());
    std::uniform_int_distribution<> distr(lower, upper);
    
    return distr(generator);
  }

private:
  Random() = delete;

};

} // ns mr

#endif /* MR_RANDOM_H */
