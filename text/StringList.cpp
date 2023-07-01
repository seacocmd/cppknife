/*
 * StringList.cpp
 *
 *  Created on: 12.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../text/text.hpp"

namespace cppknife {
StringList::StringList(size_t startSize, size_t clusterSize, size_t capacity) :
    _lines(), _nextIndex(0), _startSize(startSize), _clusterSize(clusterSize), _capacity(
        capacity), _storage(capacity) {
  _lines.reserve(startSize);
}

StringList::~StringList() {
  clear();
}

StringList::StringList(const StringList &other) :
    _lines(), _nextIndex(), _startSize(other._startSize), _clusterSize(
        other._clusterSize), _capacity(other._capacity), _storage(
        other._capacity) {
  _lines.reserve(other._lines.capacity());
}

StringList&
StringList::operator =(const StringList &other) {
  _lines = other._lines;
  _nextIndex = other._nextIndex;
  _startSize = other._startSize;
  _clusterSize = other._clusterSize;
  _capacity = other._capacity;
  _lines.reserve(other._lines.capacity());
  // _storage is yet initialized
  return *this;
}

void StringList::append(const char *line, int length) {
  if (length < 0) {
    length = strlen(line);
  }
  char *buffer = _storage.allocate(length);
  if (buffer != nullptr) {
    // Note: EOS is already set in allocate()
    memcpy(buffer, line, length);
  }
  if (_nextIndex < _lines.size()) {
    _lines[_nextIndex++] = buffer;
  } else {
    if (_lines.size() >= _lines.capacity()) {
      _lines.reserve(_lines.capacity() + _clusterSize);
    }
    _lines.push_back(buffer);
    _nextIndex = _lines.size();
  }
}

void StringList::append(const std::string &line) {
  append(line.c_str(), line.length());
}

bool StringList::empty() const {
  bool rc = _lines.empty();
  return rc;
}
const char*
StringList::at(size_t index) const {
  const char *rc = index >= 0 && index < _nextIndex ? _lines[index] : "";
  return rc;
}

void StringList::clear() {
  for (size_t ix = 0; ix < _nextIndex; ix++) {
    _lines[ix] = nullptr;
  }
  _nextIndex = 0;
  _storage.clear();
}

std::string StringList::join(const char *separator) const {
  if (separator != nullptr && separator[0] == '\0') {
    separator = nullptr;
  }
  size_t length = 0;
  size_t lengthSeparator = separator == nullptr ? 0 : strlen(separator);
  for (size_t ix = 0; ix < _nextIndex; ix++) {
    length += _storage.sizeOf(_lines[ix]) + lengthSeparator;
  }
  std::string rc;
  rc.reserve(length + 1);
  bool first = true;
  for (size_t ix = 0; ix < _nextIndex; ix++) {
    if (first) {
      first = false;
    } else {
      if (separator != nullptr) {
        rc += separator;
      }
    }
    rc += _lines[ix];
  }
  return rc;
}

} /* cppknife */
