/*
 * InternalError.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../basic/basic.hpp"

namespace cppknife {

InternalError::InternalError(const char *message, const char *arg1, int arg2) :
    _message() {
  std::string buffer(message);
  if (arg1 != nullptr) {
    appendString(buffer, " ", arg1);
  }
  if ((unsigned) arg2 != 0x80000000) {
    char buffer2[64];
    snprintf(buffer2, sizeof buffer, "%d", arg2);
    buffer += buffer2;
  }
  _message = buffer;
}
InternalError::InternalError(const std::string &message) :
    _message(copyCString(message.c_str(), message.size())) {
}

InternalError::~InternalError() {
}

InternalError::InternalError(const InternalError &other) :
    _message(other._message) {
}
InternalError&
InternalError::operator=(const InternalError &other) {
  _message = other._message;
  return *this;
}

}
/* cppknife */
