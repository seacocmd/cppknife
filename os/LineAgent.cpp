/*
 * LineAgent.cpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"

namespace cppknife {

FileBuffer::FileBuffer(size_t bufferSize = 0x10000) :
/* _staticBuffer, */
    _buffer(_staticBuffer), _bufferSize(bufferSize), _endOfBuffer(nullptr), _nextLine(
        nullptr), _restLength(0), _filePosition(0), _offsetCurrentLine(0), _lineNo(
        0) {
  if (bufferSize < sizeof _staticBuffer - 1) {
    _buffer = _staticBuffer;
  } else {
    _buffer = new char[bufferSize + 1];
  }
  _endOfBuffer = _buffer + bufferSize;
  _nextLine = _buffer;
  _endOfBuffer[0] = '\0';
}
FileBuffer::~FileBuffer() {
  if (_buffer != _staticBuffer) {
    delete _buffer;
    _buffer = nullptr;
  }
}
void FileBuffer::clone(FileBuffer &other) {
  _restLength = other._restLength;
  _nextLine = _buffer + (other._nextLine - other._buffer);
  _filePosition = other._filePosition;
  if (static_cast<size_t>(other._endOfBuffer - other._buffer) > _bufferSize) {
    increaseBuffer(other._bufferSize - _bufferSize);
  }
  _endOfBuffer = _buffer + (other._endOfBuffer - other._buffer);
}
void FileBuffer::increaseBuffer(size_t clusterSize) {
  if (clusterSize == 0) {
    clusterSize = _bufferSize;
  }
  if (_bufferSize + clusterSize < sizeof _staticBuffer - 1) {
    _bufferSize += clusterSize;
    _endOfBuffer += clusterSize;
  } else {
    _bufferSize += clusterSize;
    char *newBuffer = new char[_bufferSize + 1];
    _endOfBuffer = newBuffer + _bufferSize;
    size_t length = _nextLine - _buffer + _restLength;
    memcpy(newBuffer, _buffer, length);
    if (_buffer != _staticBuffer) {
      delete _buffer;
    }
    _buffer = newBuffer;
  }
  _endOfBuffer[0] = '\0';
}
void FileBuffer::reset() {
  _endOfBuffer = _buffer + _bufferSize;
  *_endOfBuffer = '\0';
  _nextLine = _buffer;
  _restLength = 0;
}
void FileBuffer::setBufferSize(size_t bufferSize) {
  if (bufferSize >= sizeof _staticBuffer) {
    if (_endOfBuffer == _buffer) {
      // no copy needed:
      if (bufferSize > _bufferSize) {
        auto newBuffer = new char[bufferSize + 1];
        if (_buffer != _staticBuffer) {
          delete _buffer;
        }
        _buffer = _endOfBuffer = newBuffer;
        _bufferSize = bufferSize;
      }
    } else {
      // Copy needed:
      if (bufferSize > _bufferSize) {
        auto newBuffer = new char[bufferSize + 1];
        auto oldLength = _endOfBuffer - _buffer;
        memcpy(newBuffer, _buffer, oldLength);
        _endOfBuffer = newBuffer + oldLength;
        _nextLine = newBuffer + (_nextLine - _buffer);
        if (_buffer != _staticBuffer) {
          delete _buffer;
        }
        _buffer = newBuffer;
        _bufferSize = bufferSize;
      }
    }
  } else {
    _bufferSize = bufferSize;
  }
}

LineAgent::LineAgent(Logger *logger, size_t startBufferSize, size_t clusterSize) :
    _logger(logger), _handle(-1), _filename(),
    _currentBuffer(nullptr), _previousBuffer(
        nullptr), _eofReached(false), _hasBinaryData(false), _clusterSize(
        clusterSize) {
  _currentBuffer = new FileBuffer(startBufferSize);
  _previousBuffer = new FileBuffer(startBufferSize);
}
LineAgent::~LineAgent() {
  if (_handle > 0) {
    close(_handle);
  }
  delete _currentBuffer;
  _currentBuffer = nullptr;
  delete _previousBuffer;
  _previousBuffer = nullptr;

}
int LineAgent::estimateLineCount() {
  size_t rc = -1;
  struct stat state;
  if (::stat(_filename.c_str(), &state) == 0) {
    auto fileSize = state.st_size;
    if (_currentBuffer->_restLength == 0) {
      fillBuffer(0);
    }
    size_t lines = 1;
    char *ptr = _currentBuffer->_buffer;
    char *last = ptr;
    auto restLength = _currentBuffer->_restLength;
    while ((ptr = static_cast<char*>(memchr(static_cast<void*>(last), '\n',
        restLength))) != nullptr) {
      lines++;
      restLength -= (ptr - last) - 1;
      last = ptr + 1;
    }
    rc = 1
        + int(
            static_cast<double>(fileSize) / _currentBuffer->_restLength
                * lines);
  }
  return rc;
}
char* LineAgent::previousLine(int no, size_t &size) {
  size = 0;
  return nullptr;
}
const char* LineAgent::nextLine(size_t &length) {
  const char *rc = nullptr;
  if (_handle > 0) {
    if (!_eofReached && _currentBuffer->_restLength == 0) {
      fillBuffer(0);
    }
    rc =
        _currentBuffer->_restLength == 0 ?
            _currentBuffer->_buffer : _currentBuffer->_nextLine;
    _currentBuffer->_lineNo++;
    bool again = false;
    do {
      again = false;
      if (_currentBuffer->_restLength > 0
          && (_currentBuffer->_nextLine = static_cast<char*>(memchr(
              static_cast<void*>(_currentBuffer->_nextLine), '\n',
              _currentBuffer->_restLength))) != nullptr) {
        *_currentBuffer->_nextLine = '\0';
        length = _currentBuffer->_nextLine - rc;
        if (++(_currentBuffer->_nextLine) > _currentBuffer->_endOfBuffer) {
          _currentBuffer->_nextLine = _currentBuffer->_endOfBuffer;
        }
        if (_eofReached) {
          _currentBuffer->_restLength -= length + 1;
        } else {
          _currentBuffer->_restLength = _currentBuffer->_endOfBuffer
              - _currentBuffer->_nextLine;
        }
      } else if (_eofReached) {
        if (_currentBuffer->_restLength <= 0) {
          rc = nullptr;
          length = 0;
        } else {
          length = strlen(rc);
          _currentBuffer->_nextLine += length;
          _currentBuffer->_restLength = 0;
        }
      } else if (_currentBuffer->_restLength == _currentBuffer->_bufferSize) {
        length = _currentBuffer->_restLength;
        _currentBuffer->_restLength = 0;
        _currentBuffer->resetNextLine();
      } else {
        _currentBuffer->_nextLine = const_cast<char*>(rc);
        fillBuffer(_currentBuffer->_nextLine - _currentBuffer->_buffer);
        rc = _currentBuffer->resetNextLine();
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
  _currentBuffer->_offsetCurrentLine = rc - _currentBuffer->_buffer;
  return rc;
}
void LineAgent::fillBuffer(size_t startOffset) {
  // Swap the two buffers:
  auto swap = _currentBuffer;
  _currentBuffer = _previousBuffer;
  _previousBuffer = swap;
  _currentBuffer->clone(*_previousBuffer);
  if (_currentBuffer->_restLength > 0) {
    memmove(_currentBuffer->_buffer, _previousBuffer->_buffer + startOffset,
        _previousBuffer->_restLength);
  }
  _currentBuffer->addOffset();
  _currentBuffer->_nextLine = _currentBuffer->_buffer
      + _currentBuffer->_restLength;
  ssize_t requested = _currentBuffer->_endOfBuffer - _currentBuffer->_nextLine;
  if (requested <= 0) {
    _currentBuffer->_nextLine = _currentBuffer->_buffer;
    requested = _currentBuffer->_bufferSize;
  }
  auto bytes = read(_handle, _currentBuffer->_nextLine, requested);
  if (bytes < 0) {
    bytes = 0;
  }
  if (_currentBuffer->_restLength + bytes < _currentBuffer->_bufferSize) {
    _currentBuffer->_endOfBuffer = _currentBuffer->_nextLine + bytes;
    *(_currentBuffer->_endOfBuffer) = '\0';
  }
  if (bytes > 0 && memchr(_currentBuffer->_nextLine, '\0', bytes) != nullptr) {
    _hasBinaryData = true;
  }
  _eofReached = bytes < requested;
  _currentBuffer->_restLength += bytes;
  _currentBuffer->_nextLine = _currentBuffer->_buffer;
}

size_t LineAgent::offsetCurrentLine() const {
  return _currentBuffer->_filePosition + _currentBuffer->_offsetCurrentLine;
}
bool LineAgent::openFile(const char *filename, bool checkBinary,
    bool ignoreError) {
  if (_handle > 0) {
    close(_handle);
  }
  _eofReached = false;
  _handle = open(filename, O_RDONLY);
  _filename = filename;
  _hasBinaryData = false;
  bool rc = true;
  if (_handle < 0) {
    if (!ignoreError) {
    char buffer[512];
    _logger->say(LV_ERROR,
        formatCString("cannot open: %s (%d) cwd: %s", filename, errno,
            getcwd(buffer, sizeof buffer)));
    }
    rc = false;
  }
  reset();
  if (checkBinary) {
    size_t tempSize = min(_currentBuffer->_bufferSize, 4096);
    auto bytes = read(_handle, _currentBuffer->_buffer, tempSize);
    if (bytes <= 0) {
      if (!ignoreError) {
        char buffer[512];
        _logger->say(LV_ERROR,
            formatCString("cannot read: %s (%d) cwd: %s", filename, errno,
                getcwd(buffer, sizeof buffer)));
      }
      rc = false;
    } else {
      if (memchr(_currentBuffer->_nextLine, '\0', bytes) != nullptr) {
        _hasBinaryData = true;
        rc = false;
      }
      _eofReached = static_cast<size_t>(bytes) < tempSize;
      _currentBuffer->_restLength += bytes;
      _currentBuffer->_nextLine = _currentBuffer->_buffer;
    }
  }
  return rc;
}

void LineAgent::reset() {
  _currentBuffer->reset();
  _previousBuffer->reset();
}
void LineAgent::setBufferSize(size_t bufferSize) {
  if (_currentBuffer->_nextLine != _currentBuffer->_buffer) {
    _logger->say(LV_ERROR, "setBufferSize(): illegal state. No change done.");
  } else {
    _currentBuffer->setBufferSize(bufferSize);
    _previousBuffer->setBufferSize(bufferSize);
  }
}
} /* namespace */
