//
// mr.memory_layout.h
//

#ifndef _MR_MEMORYLAYOUT_H
#define _MR_MEMORYLAYOUT_H

#include <thread>
#include <atomic>

namespace mr {

static constexpr const char *_MEMORYLAYOUT_NAME { "schwarzschildradiustm" };
static constexpr const uint32_t _MEMORYLAYOUT_DATA_LINES { 100000 };
static constexpr const uint32_t _MEMORYLAYOUT_DATA_CHARACTERS_PER_LINE { 200 };
static constexpr const uint64_t _MEMORYLAYOUT_DATA_SIZE {
    _MEMORYLAYOUT_DATA_LINES * _MEMORYLAYOUT_DATA_CHARACTERS_PER_LINE };

typedef struct {
  // lock
  pthread_mutex_t _lock;

  // data 
  uint32_t _offsetCount;
  uint64_t _offsets[mr::_MEMORYLAYOUT_DATA_LINES];
  uint64_t _offsetIndex;

  uint32_t _matchedOffsetCount;
  uint64_t _matchedOffsets[mr::_MEMORYLAYOUT_DATA_LINES];

  char _lines[mr::_MEMORYLAYOUT_DATA_SIZE];
  std::atomic<bool> _finished;

} MemoryLayout;

} // ns mr

#endif // _MR_MEMORYLAYOUT_H
