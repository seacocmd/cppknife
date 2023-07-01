/*
 * Rectangle.cpp
 *
 *  Created on: 04.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "geo.hpp"

namespace cppknife {

Rectangle::Rectangle() :
    _point(0.0, 0.0), _width(0.0), _height(0.0) {
}

Rectangle::Rectangle(double xMin, double yMin, double width, double height) :
    _point(xMin, yMin), _width(width), _height(height) {
}
Rectangle::Rectangle(const Point2D &point, double width, double height) :
    _point(point), _width(width), _height(height) {
}

Rectangle::~Rectangle() {
}

Rectangle::Rectangle(const Rectangle &other) :
    _point(other._point), _width(other._width), _height(other._height) {
}

Rectangle&
Rectangle::operator=(const Rectangle &other) {
  _point = other._point;
  _width = other._width;
  _height = other._height;
  return *this;
}

std::string Rectangle::asPointString(int precision) const {
  char buffer[256];
  if (precision < 0) {
    precision = PolyGeoConfiguration::_precision;
  }
  sprintf(buffer, "[%.*f,%.*f/%.*f,%.*f]", precision, _point._x, precision,
      _point._y, precision, _point._x + _width, precision, _point._y + _height);
  return std::string(buffer);
}
std::string Rectangle::asString(int precision) const {
  char buffer[256];
  if (precision < 0) {
    precision = PolyGeoConfiguration::_precision;
  }
  sprintf(buffer, "[%.*f,%.*f+%.*f+%.*f]", precision, _point._x, precision,
      _point._y, precision, _width, precision, _height);
  return std::string(buffer);
}
Rectangle&
Rectangle::outlineOf(Rectangle &outline) const {
  outline = *this;
  return outline;
}

bool Rectangle::contains(double x, double y, float tolerance) const {
  bool rc = betweenFloats(_point._x, x, _point._x + _width, tolerance)
      && betweenFloats(_point._y, y, _point._y + _height, tolerance);
  return rc;
}
bool Rectangle::isEqual(const Rectangle &other, float tolerance) const {
  bool rc = equalFloats(_width, other._width, tolerance)
      && equalFloats(_height, other._height, tolerance)
      && _point.isEqual(other._point, tolerance);
  return rc;
}
} /* namespace polygeo */
