/*
 * Shape.hpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef SHAPE_HPP_
#define SHAPE_HPP_
namespace cppknife {
/**
 * Defines the different types of the <em>Shape</em> instances.
 */
enum ShapeType {
  SHAPE_TYPE_UNDEF, POINT2D, RECTANGLE, POLYGON, RASTER_INFO
};
class Rectangle;
class Polygon;
/**
 * Abstract base class of a drawable object.
 */
class Shape {
public:
  Shape(ShapeType shapeType);
  virtual
  ~Shape();
  Shape();
  Shape(const Shape &other);
  Shape&
  operator=(const Shape &other);
protected:
  ShapeType _shapeType;
  ///
  /// A unique id that allows to follow instances while debugging.
  int _shapeId;
public:
  /**
   * Returns a string with the main properties.
   *
   * @param precision The number of digits behind the '.'.
   *   If &lt; 0 a default value is used: @see PolyGeoConfiguration::_precision.
   */
  virtual std::string
  asString(int precision = -1) const = 0;
  /**
   * Calculates the smallest vertical/horizontal rectangle containing all points of the instance.
   *
   * @param[out] result The result is stored here.
   * @return The parameter <em>result</em>(for chaining).
   */
  virtual Rectangle&
  outlineOf(Rectangle &result) const = 0;
public:
  ///
  /// The next created instance will have that id.
  static int _nextId;
public:
  /**
   *  Calculates the the smallest vertical/horizontal rectangle containing all points of the given shapes.
   *
   *  @param shapes An array of <em>Shape</em> instances.
   *  @param size If lower or equal 0: the array shapes must end with a null entry.<br>
   *    Otherwise: the number of entries in <em>shapes</em>
   *  @param[out] result The result is stored here.
   *  @return The parameter <em>shapes</em> (for chaining).
   */
  static Rectangle&
  combineOutlines(Shape **shapes, size_t size, Rectangle &result);
};

double toleranceOf(double value);

} /* namespace cppknife */

#endif /* SHAPE_HPP_ */
