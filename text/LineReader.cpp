/*
 * LineReader.cpp
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "text.hpp"
#include "LineReader.hpp"

namespace cppknife {

/**
 * Manages a line oriented file.
 * Allows to lookahead some lines.
 */
LineReader::LineReader(const char *filename, Logger &logger, bool removeNewline,
    size_t bufferSize) :
    _filename(filename == nullptr ? "" : filename), _logger(logger), _fileHandle(
        -1), _removeNewline(removeNewline), _lookAheadLines(), _ignoreLookahead(
        false), _currentLine(), _blockSize(bufferSize), _nextBlock(), _cursorNextBlock(
        nullptr), _countBlockReads(0) {
  _currentLine.reserve(bufferSize);
  _nextBlock.reserve(bufferSize);
  if (filename != nullptr) {
    if ((_fileHandle = ::open(filename, O_RDONLY)) < 0) {
      _logger.say(LV_ERROR,
          formatCString("cannot open (%d): %s", errno, filename));
    } else {
      readBlock();
    }
  }
}

LineReader::~LineReader() {
  if (_fileHandle >= 0) {
    ::close(_fileHandle);
    _fileHandle = -1;
  }
}

bool LineReader::endOfFile() {
  if (_cursorNextBlock == nullptr) {
    readBlock();
  }
  bool rc = !(_cursorNextBlock != nullptr || _lookAheadLines.size() > 0);
  return rc;
}

int LineReader::estimateLineCount(double factor) {
  struct stat status;
  if (stat(_filename.c_str(), &status) != 0) {
    throw cppknife::InternalError("estimateLineCount(): file not found: %s",
        _filename.c_str());
  }
  if (_cursorNextBlock == nullptr) {
    readBlock();
  }
  auto size = _nextBlock.size();
  size_t lines = countCharInCString(_nextBlock.c_str(), '\n') + 1;
  int rc = int(0.5 + (double) status.st_size / size * lines * factor);
  return rc;
}

bool LineReader::hasBinaryData(bool localOnly) {
  bool rc = false;
  if (!localOnly && _hasBinaryData) {
    rc = true;
  } else {
    if (_cursorNextBlock == nullptr) {
      readBlock();
    }
    unsigned char *ptr = (unsigned char*) _nextBlock.c_str();
    auto count = _nextBlock.size();
    unsigned char cc;
    while (count-- > 0) {
      if ((cc = *ptr++) == '0' || (cc < ' ' && (cc < '\t' || cc > '\r'))) {
        rc = true;
        break;
      }
    }
    if (rc) {
      _hasBinaryData = true;
    }
  }
  return rc;
}

const std::vector<std::string> LineReader::lookahead(int countLines) {
  _ignoreLookahead = true;
  _lookAheadLines.resize(0);
  _lookAheadLines.reserve(countLines);
  for (int ix = 0; ix < countLines; ix++) {
    if (endOfFile()) {
      break;
    } else {
      _lookAheadLines.push_back(nextLine());
    }
  }
  _ignoreLookahead = false;
  return _lookAheadLines;
}

bool LineReader::readBlock() {
  bool rc = false;
  _nextBlock.resize(_blockSize);
  auto bytes = ::read(_fileHandle, const_cast<char*>(_nextBlock.data()),
      _blockSize);
  if (bytes <= 0) {
    _nextBlock.resize(0);
    _cursorNextBlock = nullptr;
  } else {
    _nextBlock.resize(bytes);
    _cursorNextBlock = _nextBlock.c_str();
    rc = true;
    _countBlockReads++;
  }
  return rc;
}

const std::string& LineReader::nextLine() {
  if (!_ignoreLookahead && _lookAheadLines.size() > 0) {
    _currentLine = _lookAheadLines[0];
    _lookAheadLines.erase(_lookAheadLines.begin(), _lookAheadLines.begin() + 1);
  } else {
    if (!readLine()) {
      _currentLine.resize(0);
    }
  }
  return _currentLine;
}

bool LineReader::openFile(const char *filename) {
  if (_fileHandle >= 0) {
    ::close(_fileHandle);
  }
  _filename = filename;
  _fileHandle = ::open(filename, O_RDONLY);
  _lookAheadLines.clear();
  _currentLine.clear();
  _cursorNextBlock = nullptr;
  return _fileHandle >= 0;
}
bool LineReader::readLine() {
  bool rc = false;
  bool again = true;
  const char *end;
  size_t length = 0;
  _currentLine.clear();
  while (again) {
    if (_cursorNextBlock == nullptr) {
      readBlock();
    }
    if (_cursorNextBlock == nullptr) {
      break;
    }
    rc = true;
    if ((end = strchr(_cursorNextBlock, '\n')) == nullptr) {
      // Put the rest of the block at the end of the line:
      length = _nextBlock.c_str() + _nextBlock.size() - _cursorNextBlock;
      _currentLine.append(_cursorNextBlock, length);
      if (_removeNewline && _currentLine.back() == '\r') {
        // Ignore the carriage return:
        _currentLine.pop_back();
      }
      _cursorNextBlock = nullptr;
    } else {
      // '\n' found:
      again = false;
      const char *start = _cursorNextBlock;
      length = end - start + 1;
      _cursorNextBlock += length;
      if (_cursorNextBlock >= _nextBlock.c_str() + _nextBlock.size()) {
        _cursorNextBlock = nullptr;
      }
      if (_removeNewline) {
        length--;
        if (length > 0 && end[-1] == '\r') {
          length--;
        }
      }
      if (length > 0) {
        auto oldSize = _currentLine.size();
        _currentLine.resize(oldSize + length);
        memcpy((void*) (_currentLine.data() + oldSize), start, length);
      }
    }
  }
  return rc;
}
} /* namespace cppknife */

