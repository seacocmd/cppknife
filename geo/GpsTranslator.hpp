/*
 * GpsTranslator.hpp
 *
 *  Created on: 18.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

/// Conversions from GPS to some EPSG/ESDI coordinates systems and vice versa.

#ifndef GEO_GPSTRANSLATOR_HPP_
#define GEO_GPSTRANSLATOR_HPP_
#include <ogr_spatialref.h>

namespace cppknife {

/**
 * @brief Stores coordinates for non GPS coordinate systems.
 */
class EastNorthCoordinates {
public:
  double _east;
  double _north;
public:
  EastNorthCoordinates() :
      _east(0), _north(0) {
  }
  EastNorthCoordinates(double east, double north) :
      _east(east), _north(north) {
  }
};
/**
 * @brief Stores coordinates for GPS coordinate systems.
 */
class GpsCoordinates {
public:
  double _longitude;
  double _latitude;
public:
  GpsCoordinates() :
      _longitude(0), _latitude(0) {
  }
  GpsCoordinates(double latitude, double longitude) :
      _longitude(longitude), _latitude(latitude) {
  }
};
/**
 * @brief Implements a translator from GPS to some EPSG/ESDI coordinates systems and vice versa.
 */
class GpsTranslator {
protected:
  Logger &_logger;
  int _epsg;
  int _esri;
  OGRSpatialReference _other;
  OGRSpatialReference _gps;
  OGRCoordinateTransformation *_transformFromGps;
  OGRCoordinateTransformation *_transformToGps;
public:
  GpsTranslator(Logger &logger, int srid);
  virtual ~GpsTranslator();
public:
  void fromGpsRaw(const GpsCoordinates &input,
      EastNorthCoordinates &output) const;
  void toGpsRaw(const EastNorthCoordinates &input,
      GpsCoordinates &output) const;
};

} /* namespace cppknife */

#endif /* GEO_GPSTRANSLATOR_HPP_ */
