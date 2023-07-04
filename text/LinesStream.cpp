/*
 * LinesStream.cpp
 *
 *  Created on: 11.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {

LinesStream::~LinesStream() {
}
std::string LinesStream::name() const {
  return "";
}
StringLinesStream::StringLinesStream(const char *text) :
    LinesStream(), _text(text), _valid(true) {
}
StringLinesStream::StringLinesStream(const std::string &text) :
    LinesStream(), _text(text), _valid(true) {
}
StringLinesStream::~StringLinesStream() {
}
bool StringLinesStream::endOfInput() const {
  return !_valid;
}
bool StringLinesStream::fetch(std::string &text) {
  bool rc = _valid;
  if (rc) {
    text = _text;
    _valid = false;
  }
  return rc;
}

void StringLinesStream::reset() {
  _valid = true;
}
FileLinesStream::FileLinesStream(const char *filename, Logger &logger,
    bool removeNewline, size_t bufferSize) :
    LinesStream(), LineReader(filename, logger, removeNewline, bufferSize), _endOfInput(
        false) {
}
FileLinesStream::~FileLinesStream() {
}
bool FileLinesStream::fetch(std::string &text) {
  bool rc = !endOfFile();
  if (rc) {
    text = nextLine();
  } else {
    _endOfInput = true;
  }
  return rc;
}
bool FileLinesStream::endOfInput() const {
  return _endOfInput;
}
void FileLinesStream::reset() {
  _endOfInput = false;
  static_cast<LineReader*>(this)->reset();
}
}
/* namespace cppknife */
