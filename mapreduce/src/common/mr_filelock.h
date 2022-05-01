//
// mr_filelock.h
//

#ifndef MR_FILELOCK_H
#define MR_FILELOCK_H

#include <string>

#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/file.h>

#include "mr_configuration.h"
#include "mr_log.h"
#include "mr_sleep.h"

namespace mr {

class Filelock {

public:
  Filelock(const std::string &filename) :
      _filename(filename), _fd(0) {
    LOGGER_INFO("creating Filelock on  ", _filename);
  }

  bool lockFilelock() {
    uint32_t count(0);

    while (++count <= mr::Configuration::_DEFAULT_FILELOCK_RETRYLIMIT) {

      int fd { open(this->_filename.c_str(), O_RDWR | O_CREAT, S_IRWXU) };

      if (!flock(fd, LOCK_EX | LOCK_NB)) {

        struct stat stat1;

        if (!fstat(fd, &stat1)) {

          struct stat stat2;

          if (!stat(this->_filename.c_str(), &stat2)) {
            if (stat1.st_ino == stat2.st_ino) {
              this->_fd = fd;
              break;
            }

          } else {
            LOGGER_WARN("could not stat[2] filename ", _filename);
          }
        } else {
          LOGGER_WARN("could not stat[2] filename ", _filename);
        }
      } else {
        LOGGER_WARN("could not lock filename ", _filename);
      }

      mr::Sleep::sleepMedium();
      flock(fd, LOCK_UN);
      close(fd);
    }

    if (this->_fd <= 0) {
      LOGGER_ERROR("could not lock Filelock");
      return false;
    }

    LOGGER_DEBUG("locked Filelock");
    return true;
  }

  bool unlockFilelock() {
    if (this->_fd > 0) {
      LOGGER_DEBUG("unlocking Filelock");
      unlink(this->_filename.c_str());
      close(this->_fd);
      flock(this->_fd, LOCK_UN);
    }

    this->_fd = 0;
    return true;
  }

  ~Filelock() {
    LOGGER_DEBUG("destroying Filelock");
    this->unlockFilelock();
    LOGGER_DEBUG("destroyed Filelock");
  }

  inline const std::string& filename() const {
    return _filename;
  }

private:
  const std::string _filename;
  int _fd;

private:
  Filelock(const Filelock&) = delete;
  Filelock& operator=(const Filelock&) = delete;

};

}

#endif /* MR_FILELOCK_H */
