/*
 * LineAgent.cpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"

namespace cppknife {

LineAgent::LineAgent(Logger *logger) :
    _logger(logger), _handle(-1), _filename(),
    /* _staticBuffer */_buffer(_staticBuffer), _bufferSize(
        sizeof _staticBuffer - 1), _endOfBuffer(
        _staticBuffer + sizeof _staticBuffer - 1), _nextLine(_staticBuffer), _restLength(
        0), _eofReached(false), _hasBinaryData(false) {
  _staticBuffer[sizeof _nextLine - 1] = '\0';
}
LineAgent::~LineAgent() {
  if (_handle > 0) {
    close(_handle);
  }
  if (_buffer != _staticBuffer) {
    delete _buffer;
    _buffer = nullptr;
  }
}
int LineAgent::estimateLineCount() {
  size_t rc = -1;
  struct stat state;
  if (::stat(_filename.c_str(), &state) == 0) {
    auto fileSize = state.st_size;
    if (_restLength == 0) {
      fillBuffer(_buffer);
    }
    size_t lines = 1;
    char *ptr = _buffer;
    char *last = ptr;
    auto restLength = _restLength;
    while ((ptr = static_cast<char*>(memchr(static_cast<void*>(last), '\n',
        restLength))) != nullptr) {
      lines++;
      restLength -= (ptr - last) - 1;
      last = ptr + 1;
    }
    rc = 1 + int(static_cast<double>(fileSize) / _restLength * lines);
  }
  return rc;
}
const char* LineAgent::nextLine(size_t &length) {
  const char *rc = nullptr;
  if (_handle > 0) {
    rc = _restLength == 0 ? _buffer : _nextLine;
    if (!_eofReached && _restLength == 0) {
      fillBuffer(_buffer);
    }
    bool again = false;
    do {
      again = false;
      if (_restLength > 0
          && (_nextLine = static_cast<char*>(memchr(
              static_cast<void*>(_nextLine), '\n', _restLength))) != nullptr) {
        *_nextLine = '\0';
        length = _nextLine - rc;
        if (++_nextLine > _endOfBuffer) {
          _nextLine = _endOfBuffer;
        }
        if (_eofReached) {
          _restLength -= length + 1;
        } else {
          _restLength = _endOfBuffer - _nextLine;
        }
      } else if (_eofReached) {
        if (_restLength <= 0) {
          rc = nullptr;
          length = 0;
        } else {
          length = strlen(rc);
          _nextLine += length;
          _restLength = 0;
        }
      } else if (_restLength == _bufferSize) {
        length = _restLength;
        _restLength = 0;
        _nextLine = _buffer;
      } else {
        _nextLine = const_cast<char*>(rc);
        fillBuffer(_nextLine);
        rc = _nextLine = _buffer;
        again = true;
      }
    } while (again);
    if (rc != nullptr) {
      unsigned char *ptr = (unsigned char*) rc;
      auto count = length;
      unsigned char cc;
      while (count-- > 0) {
        if ((cc = *ptr) == '0' || (cc < ' ' && (cc < '\t' || cc > '\r'))) {
          _hasBinaryData = true;
          break;
        }
      }
    }
  }
  return rc;
}
void LineAgent::fillBuffer(const char *start) {
  if (_restLength > 0) {
    memmove(_buffer, start, _restLength);
  }
  _nextLine = _buffer + _restLength;
  ssize_t requested = _endOfBuffer - _nextLine;
  auto bytes = read(_handle, _nextLine, requested);
  if (bytes < 0) {
    bytes = 0;
  }
  if (_restLength + bytes < _bufferSize) {
    _endOfBuffer = _nextLine + bytes;
    *_endOfBuffer = '\0';
  }
  if (bytes > 0 && memchr(_nextLine, '\0', bytes) != nullptr) {
    _hasBinaryData = true;
  }
  _eofReached = bytes < requested;
  _restLength += bytes;
  _nextLine = _buffer;
}
bool LineAgent::openFile(const char *filename) {
  if (_handle > 0) {
    close(_handle);
  }
  _eofReached = false;
  _handle = open(filename, O_RDONLY);
  _filename = filename;
  _hasBinaryData = false;
  bool rc = true;
  if (_handle < 0) {
    char buffer[512];
    _logger->say(LV_ERROR,
        formatCString("cannot read: %s (%d) cwd: %s", filename, errno,
            getcwd(buffer, sizeof buffer)));
    rc = false;
  }
  reset();
  return rc;
}

void LineAgent::reset() {
  _endOfBuffer = _buffer + _bufferSize;
  *_endOfBuffer = '\0';
  _nextLine = _buffer;
  _restLength = 0;
}

void LineAgent::setBufferSize(size_t bufferSize) {
  if (_nextLine != _staticBuffer) {
    _logger->say(LV_ERROR, "setBufferSize(): illegal state. No change done.");
  } else if (bufferSize >= sizeof _staticBuffer) {
    if (_buffer != _staticBuffer) {
      delete _buffer;
    }
    _buffer = new char[bufferSize + 1];
    _bufferSize = bufferSize;
    reset();
  } else {
    _bufferSize = bufferSize;
    reset();
  }
}
} /* namespace */
