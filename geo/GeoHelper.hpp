/*
 * GeoHelper.hpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef GEO_HELPER_HPP_
#define GEO_HELPER_HPP_

namespace cppknife {
/**
 * Stores some configuration data for the package.
 */
class PolyGeoConfiguration {
public:
  ///
  /// the default precision for output of coordinates (number of digits behind the '.').
  static int _precision;
  ///
  /// The relative factor of precision on float comparison.
  static double _toleranceFactor;
public:

};

/**
 * Tests whether a given float is in a given interval with a given precision.
 * @param lowerBound The lower bound of the interval.
 * @param x The float to test.
 * @param upperBound The upper bound of the interval.
 * @param tolerance The maximum of difference for equality.
 * @return true: <em>x</em> is between <em>a</em> and <em>b</em>.
 */
inline bool betweenFloats(double lowerBound, double x, double upperBound,
    double tolerance) {
  bool rc = lowerBound >= x ? lowerBound - x <= tolerance :
            x < upperBound ? true : x - upperBound <= tolerance;
  return rc;

}
/**
 * Tests whether a given float lays in a given interval with a given tolerance
 * and not at the end points.
 * @param lowerBound The lower bound of the interval.
 * @param x The float to test.
 * @param upperBound The upper bound of the interval.
 * @param tolerance The maximum of difference for equality.
 * @return true: <em>x</em> is between <em>a</em> and <em>b</em>.
 */
inline bool betweenFloatsExclusive(double lowerBound, double x,
    double upperBound, double tolerance) {
  bool rc = lowerBound + tolerance <= x and x <= upperBound - tolerance;
  return rc;
}
/**
 * Compares two floats with a given precision.
 * @param a The first float to compare.
 * @param b The second float to compare.
 * @param tolerance The maximum of difference for equality.
 * @return true: <em>a</em> is equal to <em>b</em>
 */
inline bool equalFloats(double a, double b, double tolerance) {
  bool rc = a > b ? a - b <= tolerance : b - a <= tolerance;
  return rc;
}
/**
 * Compares two floats with a given precision.
 * @param a The first float to compare.
 * @param b The second float to compare.
 * @param tolerance The maximum of difference for equality.
 * @return true: <em>a</em> is lower or equal <em>b</em>.
 */
inline bool lowerEqualFloats(double a, double b, double tolerance) {
  bool rc = a <= b ? true : a - b <= tolerance;
  return rc;
}
/**
 * Compares two floats with a given precision.
 * @param a The first float to compare.
 * @param b The second float to compare.
 * @param tolerance The maximum of difference for equality.
 * @return true: <em>a</em> is lower as <em>b</em>.
 */
inline bool lowerFloats(double a, double b, double tolerance) {
  bool rc = a > b ? false : a + tolerance < b;
  return rc;
}
/**
 * Returns the absolute value of the precision of comparisons.
 * @param a The value to inspect.
 * @return <em>a * _toleranceFactor</em>
 */
static inline double toleranceOf(double a) {
  return a * PolyGeoConfiguration::_toleranceFactor;
}

} /* namespace polygeo */
#endif /* GEO_HELPER_HPP_ */
