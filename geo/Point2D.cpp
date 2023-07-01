/*
 * Point2D.cpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "geo.hpp"

namespace cppknife {

Point2D::Point2D() :
    Shape(POINT2D), _x(0.0), _y(0.0) {
}

Point2D::Point2D(double x, double y) :
    Shape(POINT2D), _x(x), _y(y) {
}

Point2D::~Point2D() {
}

Point2D::Point2D(const Point2D &other) :
    _x(other._x), _y(other._y) {
}

Point2D&
Point2D::operator=(const Point2D &other) {
  _x = other._x;
  _y = other._y;
  return *this;
}

bool Point2D::operator==(const Point2D &other) const {
  bool rc = other._x == _x && other._y == _y;
  return rc;
}
bool Point2D::operator!=(const Point2D &other) const {
  bool rc = other._x != _x || other._y != _y;
  return rc;
}

std::string Point2D::asString(int precision) const {
  if (precision < 0) {
    precision = PolyGeoConfiguration::_precision;
  }
  char buffer[256];
  sprintf(buffer, "[%.*f,%.*f]", precision, _x, precision, _y);
  auto rc = std::string(buffer);
  return rc;
}

Point2D* Point2D::fromList(float *list) {
  Point2D *rc = new Point2D(list[0], list[1]);
  return rc;
}

bool Point2D::isEqual(const Point2D &other, double tolerance) const {
  bool rc = equalFloats(_x, other._x, tolerance)
      && equalFloats(_y, other._y, tolerance);
  return rc;
}

Rectangle& Point2D::outlineOf(Rectangle &result) const {
  result._point = *this;
  result._width = result._height = 0.0;
  return result;
}
} /* namespace polygeo */
