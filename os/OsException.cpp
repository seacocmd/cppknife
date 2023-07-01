/*
 * OsException.cpp
 *
 *  Created on: 15.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"

namespace cppknife
{
  OsException::~OsException ()
  {
    delete _message;
    _message = nullptr;
  }

} /* cppknife */
