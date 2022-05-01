//
// mr_memory.h
//

#ifndef _MR_MEMORY_H
#define _MR_MEMORY_H

#include <thread>
#include <string>
#include <vector>
#include <climits>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <vector>

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/mman.h>

#include "mr_memorylayout.h"
#include "mr_global.h"
#include "mr_configuration.h"
#include "mr_log.h"
#include "mr_sleep.h"

namespace mr {

class Memory {
public:

  Memory(const std::string &name) :
#ifdef _MR_CLIENT_BUILD
  _name(name), _fd(shm_open(_name.c_str(), O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)), _raw(static_cast<void*>(0)),
#elif _MR_COORDINATOR_BUILD 
  _name(name), _fd(shm_open(_name.c_str(), O_RDWR, S_IRUSR | S_IWUSR)), _raw(static_cast<void*>(0)),
#endif  
          _memoryLayout(static_cast<mr::MemoryLayout*>(0)) {

    if (_fd <= 0) {
      LOGGER_ERROR("memory layout open failed ", _fd);
      return;
    }

    LOGGER_DEBUG("created Memory");
    return;
  }

  bool lockMemory() {
    LOGGER_TRACE("locking memory");

    if (pthread_mutex_lock(&this->_memoryLayout->_lock)) {
      LOGGER_ERROR("could not lock memory");
      return false;
    }

    LOGGER_TRACE("locked memory");
    return true;
  }

  bool unlockMemory() {
    LOGGER_TRACE("unlocking memory");

    if (pthread_mutex_unlock(&this->_memoryLayout->_lock)) {
      LOGGER_ERROR("unlocking mutex failed to unlock");
      return false;
    }

    LOGGER_TRACE("unlocked memory");
    return true;
  }

  ~Memory() {
    LOGGER_DEBUG("destroying Memory");

    unlockMemory();
    resetMemory();

#ifdef _MR_CLIENT_BUILD
    if (_fd) {
      if(shm_unlink(this->_name.c_str())) {
        LOGGER_ERROR("Memory shm_unlink failed");
      }
    }
#endif

    resetFD();

    LOGGER_DEBUG("destroyed Memory");
  }

  bool writeLine(const char *originalLine) {
    static bool failed(false);

    if (failed) {
      LOGGER_ERROR("shared memory was failed");
      return false;
    }

    if (this->_memoryLayout->_offsetCount + 1 >= mr::_MEMORYLAYOUT_DATA_LINES) {
      LOGGER_ERROR("shared memory indexes exhausted");
      failed = true;
      return false;
    }

    const uint64_t originalLength { strlen(originalLine) };

    const uint32_t trimCount =
        (((originalLength >= 2 && originalLine[originalLength - 2] == '\r')
            || (originalLength >= 1 && originalLine[originalLength - 1] == '\r')) ?
            1 : 0)
            + (((originalLength >= 2 && originalLine[originalLength - 2] == '\n')
                || (originalLength >= 1
                    && originalLine[originalLength - 1] == '\n')) ? 1 : 0);

    char *resultLine { strncpy(
        this->_memoryLayout->_lines + this->_memoryLayout->_offsetIndex,
        originalLine, originalLength - trimCount) };

    const uint64_t resultLength { strlen(resultLine) };

    if (this->_memoryLayout->_offsetIndex + resultLength + 1
        > mr::_MEMORYLAYOUT_DATA_SIZE) {
      LOGGER_ERROR("shared memory exhausted");
      failed = true;
      return false;
    }

    if (resultLength != (originalLength - trimCount)) {
      LOGGER_ERROR("write failed");
      failed = true;
      return false;
    }

    this->_memoryLayout->_offsets[this->_memoryLayout->_offsetCount] =
        this->_memoryLayout->_offsetIndex;
    this->_memoryLayout->_offsetIndex = this->_memoryLayout->_offsetIndex + resultLength
        + 1;
    this->_memoryLayout->_offsetCount = this->_memoryLayout->_offsetCount + 1;

    LOGGER_DEBUG("processed ", this->_memoryLayout->_offsetCount);

    return true;
  }

  void dump() {
    std::ostringstream os;

    os << "{";
    for (uint64_t i(0);
        i < mr::_MEMORYLAYOUT_DATA_LINES
            && i < this->_memoryLayout->_offsetCount; ++i) {

      if (i) {
        os << ",";
      }

      os << "[" << i + 1 << "] " << this->_memoryLayout->_offsets[i] << "/"
          << &this->_memoryLayout->_lines[this->_memoryLayout->_offsets[i]]
          << "/"
          << this->_memoryLayout->_offsets[i + 1]
              - (this->_memoryLayout->_offsets[i] + 1);

      os << std::endl;
    }
    os << "}";

    LOGGER_INFO(os.str());
  }

  bool isFinished() {
    return this->_memoryLayout->_finished;
  }

  void setFinished() {
    this->_memoryLayout->_finished.store(true);
  }

  void load(std::vector<int64_t>& results) {
    for (int64_t i(0); i<this->_memoryLayout->_matchedOffsetCount; i++) {
      results.push_back(this->_memoryLayout->_matchedOffsets[i]);
    }
  }
  
  bool writeResult(std::vector<int64_t> &results) {
    
    for (int64_t result : results) {
      this->_memoryLayout->_matchedOffsets[this->_memoryLayout->_matchedOffsetCount] = result;
      this->_memoryLayout->_matchedOffsetCount++;
    }
    
    results.clear();

    return true;
  }

  bool init() {
    LOGGER_DEBUG("started init with fd ", _fd);

    if (_fd <= 0) {
      LOGGER_ERROR("shared memory layout fd failed ", _fd);
      return false;
    }

#ifdef _MR_CLIENT_BUILD    
      if (ftruncate(_fd, sizeof(mr::MemoryLayout))) {
        LOGGER_ERROR("share ftruncate failed");
        resetFD();

        return false;
      }
#endif

    this->_raw = (mmap(NULL, sizeof(mr::MemoryLayout), PROT_READ | PROT_WRITE,
        MAP_SHARED, this->_fd, 0));
    //_raw = (mmap(NULL, sizeof(Memory::MemoryLayout), PROT_READ, MAP_SHARED, this->_fd, 0));

    if (!this->_raw) {
      LOGGER_ERROR("share mmap failed");
      resetFD();
      return false;
    } else {
      LOGGER_TRACE("mmap to raw ok ", static_cast<void*>(this->_raw));
    }

#ifdef _MR_CLIENT_BUILD
      this->_memoryLayout = new (this->_raw) (mr::MemoryLayout);
#elif _MR_COORDINATOR_BUILD
      this->_memoryLayout = static_cast<mr::MemoryLayout*>(this->_raw);
#endif

    if (!this->_memoryLayout) {
      LOGGER_ERROR("memory layout cast failed");
      resetFD();
      resetMemory();
      return false;
    } else {
      LOGGER_TRACE("mmap to memory layout ok ",
          static_cast<void*>(this->_memoryLayout));
    }

#ifdef _MR_CLIENT_BUILD
      this->_memoryLayout->_offsetCount = 0;
      this->_memoryLayout->_offsetIndex = 0;
      memset(this->_memoryLayout->_offsets, 0, sizeof(this->_memoryLayout->_offsets));
      memset(this->_memoryLayout->_lines, 0, sizeof(this->_memoryLayout->_lines));
      this->_memoryLayout->_finished.store(false);
      this->_memoryLayout->_matchedOffsetCount = 0;
      memset(this->_memoryLayout->_matchedOffsets, 0, sizeof(this->_memoryLayout->_matchedOffsets));

      pthread_mutexattr_t mutex_attr;
      pthread_mutexattr_init(&mutex_attr);
      pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED);
      pthread_mutex_init(&this->_memoryLayout->_lock, &mutex_attr);
#endif

    close(this->_fd);
    return true;
  }

  uint32_t offsetCount() const {
    if (!_memoryLayout) {
      LOGGER_ERROR("memory layout not set");
      return 0;
    }

    return this->_memoryLayout->_offsetCount;
  }

  mr::MemoryLayout* memoryLayout() {
    if (!_memoryLayout) {
      LOGGER_ERROR("memory layout not set");
    }

    return this->_memoryLayout;
  }

private:
  void resetFD() {
    LOGGER_DEBUG("resetting FD");
    if (this->_fd > 0) {
      close(this->_fd);
    }
    _fd = 0;
    LOGGER_DEBUG("reset FD");
  }

  void resetMemory() {
    LOGGER_DEBUG("resetting memory");
    if (this->_raw) {
      munmap(this->_raw, sizeof(mr::MemoryLayout));
    }
    _raw = static_cast<void*>(0);
    _memoryLayout = static_cast<mr::MemoryLayout*>(0);
    LOGGER_DEBUG("reset memory");
  }

private:
  Memory(const Memory&) = delete;
  Memory& operator=(const Memory&) = delete;

private:
  const std::string _name;
  int _fd;
  void *_raw; // raw shared memory
  mr::MemoryLayout *_memoryLayout; // layout mapped shared memory
};

class MemoryRAIIUnlockerWrapper {
public:
  MemoryRAIIUnlockerWrapper(std::shared_ptr < mr::Memory>& memory) : _memory(memory) {
  }
  
  bool lockMemory() {
    return _memory->lockMemory();
  }
  
  ~MemoryRAIIUnlockerWrapper() {
    if (_memory.get()) {
      _memory->unlockMemory();
    }
  }
  
private:
  std::shared_ptr < mr::Memory > _memory;
  
private:
  MemoryRAIIUnlockerWrapper(const MemoryRAIIUnlockerWrapper&) = delete;  
  MemoryRAIIUnlockerWrapper& operator=(const MemoryRAIIUnlockerWrapper&) = delete;

};

}

#endif /* _MR_MEMORY_H */
