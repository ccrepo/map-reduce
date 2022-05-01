//
// mr_datafile.h
//

#ifndef _MR_DATAFILE_H
#define _MR_DATAFILE_H

#include <string>
#include <fstream>

#include <sys/stat.h>

namespace mr {

class Datafile {

public:
  Datafile(const std::string &filename) :
      _filename(filename), _file(_filename.c_str()), _linesLoaded(0) {
  }

  ~Datafile() {
    this->_file.close();
  }

  bool eof() {
    return this->_file.peek() == EOF;
  }

  const char* nextline() {
    static std::string line;
    static const char* empty { "" };

    if (!this->_file.is_open() || this->eof()) {
      return empty;
    }

    line.clear();

    if (getline(this->_file, line)) {
      _linesLoaded++;
      return line.c_str();
    }

    return empty;
  }
  
  inline const std::string& filename() const {
    return _filename;
  }

  inline uint32_t linesLoaded() const {
    return _linesLoaded;
  }
  
private:
  Datafile(const Datafile&) = delete;
  Datafile& operator=(const Datafile&) = delete;

private:
  const std::string _filename;
  std::ifstream _file;
  uint32_t _linesLoaded;
};

}

#endif /* _MR_DATAFILE_H */
