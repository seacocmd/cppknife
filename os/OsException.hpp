/*
 * OsException.hpp
 *
 *  Created on: 15.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_OSEXCEPTION_HPP_
#define OS_OSEXCEPTION_HPP_

namespace cppknife
{
/**
 * @brief An exception used for operating system errors.
 */
  class OsException
  {
    const char *_message;
  public:
    OsException (const char *message): _message(copyCString(message)){
    }
    OsException (std::string message): _message(copyCString(message.c_str())){
    }
    virtual
    ~OsException ();
    OsException (const OsException &other) :
	_message (copyCString (other._message))
    {
    }
    OsException&
    operator= (const OsException &other)
    {
      _message = copyCString (other._message);
      return *this;
    }
    inline
    const char* message() const{
      return _message;
    }
  };

} /* cppknife */

#endif /* OS_OSEXCEPTION_HPP_ */
