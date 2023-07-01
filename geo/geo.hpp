/**
 * polygeo.hpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef GEO_HPP_
#define GEO_HPP_
#include "../core/core.hpp"
#include "GeoHelper.hpp"
#include "GpsTranslator.hpp"
#include "GeoException.hpp"
#include "Shape.hpp"
#include "Point2D.hpp"
#include "Rectangle.hpp"
namespace cppknife {
enum Position {
  POSITION_UNDEF, LEFT_DOWN, LEFT_UP, RIGHT_UP, RIGHT_DOWN
};
}
#endif /* GEO_HPP_ */
