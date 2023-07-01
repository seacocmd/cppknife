/*
 * GeoException.hpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef GEOEXCEPTION_HPP_
#define GEOEXCEPTION_HPP_
namespace cppknife {
/**
 * General exception for this package.
 */
class GeoException {
public:
  GeoException();
  /**
   * Constructor.
   *
   * @param message Explains the exception.
   */
  GeoException(const char *message);
  virtual
  ~GeoException();
  GeoException(const GeoException &other);
  GeoException&
  operator=(const GeoException &other);
public:
  ///
  /// Stores the description. Is a deep copy of the parameter in the constructor.
  const char *_message;
};

}

#endif /* GEOEXCEPTION_HPP_ */
