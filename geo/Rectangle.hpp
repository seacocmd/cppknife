/*
 * Rectangle.hpp
 *
 *  Created on: 04.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef RECTANGLE_HPP_
#define RECTANGLE_HPP_

/// Implements a rectangle with vertical/horizonal lines.
namespace cppknife {
/**
 * @brief Stores a vertical/horizontal rectangle.
 */
class Rectangle: public Shape {
public:
  Rectangle();
  /**
   * Constructor.
   * @param xMin The x coordinate of the left down corner.
   * @param yMin The y coordinate of the left down corner.
   * @param width The (horizontal) width.
   * @param height The (vertical) height.
   */
  Rectangle(double xMin, double yMin, double width, double height);
  /**
   * Constructor.
   * @param point The left down corner.
   * @param width The (horizontal) width.
   * @param height The (vertical) height.
   */
  Rectangle(const Point2D &point, double width, double height);
  virtual
  ~Rectangle();
  Rectangle(const Rectangle &other);
  Rectangle&
  operator=(const Rectangle &other);
public:
  /**
   * Returns a string with the two diagonal vertices.
   *
   * @param precision The number of digits behind the '.'.
   *   If &lt; 0 a default value is used: @see PolyGeoConfiguration::_precision.
   */
  std::string asPointString(int precision = -1) const;
public:
  virtual std::string
  asString(int precision = -1) const;
  virtual Rectangle&
  outlineOf(Rectangle &outline) const;
public:
  /**
   * Tests whether a given point is inside the rectangle.
   * @param x The x coordinate of the point to inspect.
   * @param y The y coordinate of the point to inspect.
   * @param tolerance The precision of the comparison.
   * @return true: the point is inside or on the sides of the rectangle.
   */
  bool contains(double x, double y, float tolerance) const;
  /**
   * Tests whether two rectangles are equal with a given precision.
   * @param other The rectangle to compare.
   * @param tolerance The precision of the comparison.
   */
  bool isEqual(const Rectangle &other, float tolerance) const;
public:
  Point2D _point;
  double _width;
  double _height;
public:

};

} /* namespace polygeo */

#endif /* RECTANGLE_HPP_ */
