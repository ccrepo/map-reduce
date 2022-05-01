//
// mr_algorithm.h
//

#ifndef MR_ALGORITHM_H
#define MR_ALGORITHM_H

#include <vector>
#include <tuple>
#include <algorithm>

namespace mr {

class Algorithm {

public:
  static bool partition(
      uint64_t numberOfElements, uint64_t numberOfSets, std::vector<std::tuple<uint64_t, uint64_t>>& result, bool idempotentOnFail=true);

private:
  Algorithm() = delete;

};

} // ns mr

#endif /* MR_ALGORITHM_H */
