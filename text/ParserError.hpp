/*
 * ParserError.hpp
 *
 *  Created on: 03.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_PARSERERROR_HPP_
#define CORE_PARSERERROR_HPP_

namespace cppknife {
/// Represents an error in parsing process and show the error position in the input.
/**
 * Represents an error in parsing process and show the error position in the input.
 */
class ParserError {
  std::string _message;
public:
  ParserError(const char *message, const Parser &parser);
  ParserError(const std::string &message, const Parser &parser);
  virtual
  ~ParserError();
public:
  const char* message() const {
    return _message.c_str();
  }
};

} /* cppknife */

#endif /* CORE_PARSERERROR_HPP_ */
