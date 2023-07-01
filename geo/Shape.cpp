/*
 * Shape.cpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "geo.hpp"

namespace cppknife {
int Shape::_nextId = 0;

Shape::Shape(ShapeType shapeType) :
    _shapeType(shapeType), _shapeId(++_nextId) {
}

Shape::~Shape() {
}

Shape::Shape() :
    _shapeType(SHAPE_TYPE_UNDEF), _shapeId(++_nextId) {
}
Shape::Shape(const Shape &other) {
  _shapeType = SHAPE_TYPE_UNDEF;
}
Shape&
Shape::operator=(const Shape &other) {
  _shapeType = other._shapeType;
  return *this;
}

void outlineOf(std::vector<Shape> shapes, Rectangle &result) {
  throw new GeoException("not implemented");
}

} /* namespace geometry */
