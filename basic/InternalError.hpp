/*
 * InternalError.hpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_INTERNALERROR_HPP_
#define CORE_INTERNALERROR_HPP_

namespace cppknife {
/**
 * Represents a internal error: unexpected states, limit violations...
 */
class InternalError {
  std::string _message;
public:
  InternalError(const char *message, const char *arg1 = nullptr, int arg2 =
      0x80000000);
  InternalError(const std::string &message);
  virtual
  ~InternalError();
  InternalError(const InternalError &other);
  InternalError&
  operator=(const InternalError &other);
public:
  const char* message() const {
    return _message.c_str();
  }
};

} /* cppknife */

#endif /* CORE_INTERNALERROR_HPP_ */
