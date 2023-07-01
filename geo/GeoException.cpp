/*
 * GeoException.cpp
 *
 *  Created on: 03.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "geo.hpp"

namespace cppknife {
GeoException::GeoException() :
    _message(nullptr) {
}
GeoException::~GeoException() {
  if (_message != nullptr) {
    free((void*) _message);
    _message = nullptr;
  }
}
GeoException::GeoException(const char *message) :
    _message((const char*) strdup(message)) {
}

GeoException::GeoException(const GeoException &other) :
    _message(strdup(other._message)) {
}

GeoException&
GeoException::operator=(const GeoException &other) {
  _message = strdup(other._message);
  return *this;
}

}

