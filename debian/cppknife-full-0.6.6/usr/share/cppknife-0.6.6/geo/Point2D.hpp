/*
 * Point2D.hpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef POINT2D_HPP_
#define POINT2D_HPP_

/// Implements a point in a two dimension context.
namespace cppknife
{

  /**
 * @brief Manages a two dimensional point.
   */
  class Point2D : public Shape
  {
  public:
    Point2D ();
    /**
     * Constructor.
     * @param x The x coordinate (left to right)
     * @param y The y coordinate (down to up)
     */
    Point2D (double x, double y);
    virtual
    ~Point2D ();
    Point2D (const Point2D &other);
    Point2D&
    operator= (const Point2D &other);
    bool
    operator== (const Point2D &other) const;
    bool
    operator!= (const Point2D &other) const;
  public:
    virtual std::string
    asString (int precision = -1) const;
    virtual Rectangle&
    outlineOf (Rectangle &outline) const;
  public:
    /**
     * Tests the equality of two points with a given tolerance.
     * @param other The other point to compare (with the instance).
     * @param tolerance If the difference of the coordinates is lower than that the points are equal.
     * @return true: The points are equal.
     */
    bool
    isEqual (const Point2D &other, double tolerance) const;
  public:
    double _x;
    double _y;
  public:
    /**
     * Builds a point by reading the coordinates from an array of floats.
     * @param list An array with at least 2 floats: x and y.
     * @return A <em>Point2D</em> instance.
     */
    static Point2D*
    fromList (float *list);
  };

} /* namespace polygeo */

#endif /* POINT2D_HPP_ */
