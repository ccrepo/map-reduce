#include <iostream>
#include <fcntl.h>

#include <cstring>
#include <cstdint>
#include <csignal>
#include <climits>
#include <errno.h>

#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>

#include "../src/common/mr_memorylayout.h"
using namespace mr;

#ifdef _RUNTYPE_READONLY
#elif _RUNTYPE_READWRITE
#else
static_assert(0, "compilation MUST include -D_RUNTYPE_READONLY or -D_RUNTYPE_READWRITE !!");
#endif
  
int main(int argc, const char *argv[]) {

#ifdef _RUNTYPE_READONLY
  int fd(shm_open("schwarzschildradius", O_RDONLY, S_IRUSR | S_IWUSR));
#elif _RUNTYPE_READWRITE
  int fd(shm_open("schwarzschildradius", O_RDWR, S_IRUSR | S_IWUSR));
#else
  int fd;
#endif
  
  std::cout << "fd is " << fd << std::endl;

#ifdef _RUNTYPE_READONLY
  void *rawPtr = (mmap(NULL, sizeof(mr::MemoryLayout), PROT_READ, MAP_SHARED,fd, 0));
#elif _RUNTYPE_READWRITE
  void *rawPtr { (mmap(NULL, sizeof(mr::MemoryLayout), PROT_READ | PROT_WRITE, MAP_SHARED,
      fd, 0)) };  
#else 
  void *rawPtr;
#endif

  std::cout << "rawPtr " << static_cast<void*>(rawPtr) << std::endl;

  MemoryLayout *memoryLayout = static_cast<mr::MemoryLayout*>(rawPtr);

  std::cout << "memoryLayout " << static_cast<void*>(memoryLayout) << std::endl;
  
  if (memoryLayout) {
    std::cerr << "offsetCount " << memoryLayout->_offsetCount << std::endl;    
  }

  return 1;
}
