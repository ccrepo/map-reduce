//
// mr_algorithm.cpp
//

#include <memory>

#include "mr_algorithm.h"
#include "mr_log.h"

namespace mr {

bool Algorithm::partition(uint64_t numberOfElements, uint64_t numberOfSets,
    std::vector<std::tuple<uint64_t, uint64_t>> &result, bool idempotentOnFail) {

  std::unique_ptr<std::vector<std::tuple<uint64_t, uint64_t>>> buffer;
  
  if (idempotentOnFail) {
    buffer.reset(new std::vector<std::tuple<uint64_t, uint64_t>>());
    buffer->reserve(numberOfSets);
  }
  
  if (!numberOfSets) {
    LOGGER_ERROR("setsCount is zero");
    return false;
  }

  uint64_t whole(0);

  if (numberOfElements >= numberOfSets) {
    whole = numberOfElements / numberOfSets;
  }

  uint64_t part(numberOfElements % numberOfSets);

  uint64_t index(0);

  for (uint64_t i(0); i < numberOfSets; i++) {

    auto lower(index);

    auto upper(lower + whole + (i < part ? 1 : 0));

    if ((upper - lower) < 0) {
      return false;
    }

    if ((upper - lower) > 0) {
      if (idempotentOnFail) {
        buffer->push_back(std::tuple<uint64_t, uint64_t>(lower, upper));
      } else {
        result.push_back(std::tuple<uint64_t, uint64_t>(lower, upper));
      }
    }

    index = upper;
  }

  if (idempotentOnFail) {
    std::copy(buffer->begin(), buffer->end(), std::back_inserter(result));
  }
  
  return true;
}

}
