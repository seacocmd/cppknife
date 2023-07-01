/*
 * GpsTranslator.cpp
 *
 *  Created on: 18.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

/**
 * Debian package: libgdal-dev
 * see: /usr/include/gdal and /usr/lib/libgdal.a
 */
#include "geo.hpp"
std::map<int, std::string> s_esriData(
    {
        { 102004,
            R"""(PROJCS["USA_Contiguous_Lambert_Conformal_Conic",
GEOGCS["NAD83",
  DATUM["North_American_Datum_1983",
      SPHEROID["GRS 1980",6378137,298.257222101,
          AUTHORITY["EPSG","7019"]],
      AUTHORITY["EPSG","6269"]],
  PRIMEM["Greenwich",0,
      AUTHORITY["EPSG","8901"]],
  UNIT["degree",0.0174532925199433,
      AUTHORITY["EPSG","9122"]],
  AUTHORITY["EPSG","4269"]],
PROJECTION["Lambert_Conformal_Conic_2SP"],
PARAMETER["latitude_of_origin",39],
PARAMETER["central_meridian",-96],
PARAMETER["standard_parallel_1",33],
PARAMETER["standard_parallel_2",45],
PARAMETER["false_easting",0],
PARAMETER["false_northing",0],
UNIT["metre",1,
  AUTHORITY["EPSG","9001"]],
AXIS["Easting",EAST],
AXIS["Northing",NORTH],
AUTHORITY["ESRI","102004"]]
)""" },
        { 102024,
            R"""(PROJCS["Africa_Lambert_Conformal_Conic",
GEOGCS["WGS 84",
  DATUM["WGS_1984",
      SPHEROID["WGS 84",6378137,298.257223563,
          AUTHORITY["EPSG","7030"]],
      AUTHORITY["EPSG","6326"]],
  PRIMEM["Greenwich",0,
      AUTHORITY["EPSG","8901"]],
  UNIT["degree",0.0174532925199433,
      AUTHORITY["EPSG","9122"]],
  AUTHORITY["EPSG","4326"]],
PROJECTION["Lambert_Conformal_Conic_2SP"],
PARAMETER["latitude_of_origin",0],
PARAMETER["central_meridian",25],
PARAMETER["standard_parallel_1",20],
PARAMETER["standard_parallel_2",-23],
PARAMETER["false_easting",0],
PARAMETER["false_northing",0],
UNIT["metre",1,
  AUTHORITY["EPSG","9001"]],
AXIS["Easting",EAST],
AXIS["Northing",NORTH],
AUTHORITY["ESRI","102024"]]AUTHORITY["ESRI","102004"]]
)""" },
        { 102030,
            R"""(PROJCS["Asia_South_Lambert_Conformal_Conic",
GEOGCS["WGS 84",
  DATUM["WGS_1984",
      SPHEROID["WGS 84",6378137,298.257223563,
          AUTHORITY["EPSG","7030"]],
      AUTHORITY["EPSG","6326"]],
  PRIMEM["Greenwich",0,
      AUTHORITY["EPSG","8901"]],
  UNIT["degree",0.0174532925199433,
      AUTHORITY["EPSG","9122"]],
  AUTHORITY["EPSG","4326"]],
PROJECTION["Lambert_Conformal_Conic_2SP"],
PARAMETER["latitude_of_origin",-15],
PARAMETER["central_meridian",125],
PARAMETER["standard_parallel_1",7],
PARAMETER["standard_parallel_2",-32],
PARAMETER["false_easting",0],
PARAMETER["false_northing",0],
UNIT["metre",1,
  AUTHORITY["EPSG","9001"]],
AXIS["Easting",EAST],
AXIS["Northing",NORTH],
AUTHORITY["ESRI","102030"]]
)""" } });

namespace cppknife {

GpsTranslator::GpsTranslator(Logger &logger, int srid) :
    _logger(logger), _epsg(srid < 10000 ? srid : 0), _esri(
        srid >= 10000 ? srid : 0), _other(), _gps(), _transformFromGps(nullptr), _transformToGps(
        nullptr) {
  const char *error1 = nullptr;
  const char *error2 = nullptr;

  if (_epsg > 0) {
    _other.importFromEPSG(_epsg);
  } else if (_esri > 0) {
    auto data = s_esriData;
    if (s_esriData.find(_esri) == s_esriData.end()) {
      auto msg = formatCString("GpsTranslator: unknown ESRI: ", srid);
      _logger.say(LV_ERROR, msg);
      throw InternalError(msg);
    }
    std::string &esri = s_esriData[_esri];
    _other.SetFromUserInput(esri.c_str());
    _other.morphFromESRI();
  } else {
    auto msg = formatCString("unknown SRID: ", srid);
    _logger.say(LV_ERROR, msg);
    throw InternalError(msg);
  }
  _gps.SetWellKnownGeogCS("EPSG:4326");
  _transformFromGps = OGRCreateCoordinateTransformation(&_gps, &_other);
  error1 = CPLGetLastErrorMsg();
  if (error1 != nullptr && *error1 != '\0') {
    _logger.say(LV_ERROR,
        formatCString("GpsTranslator(%d): cannot create _transformFromGps: %s",
            srid, error1));
  }
  _transformToGps = OGRCreateCoordinateTransformation(&_other, &_gps);
  error2 = CPLGetLastErrorMsg();
  if (error2 != nullptr && *error2 != '\0') {
    _logger.say(LV_ERROR,
        formatCString("GpsTranslator(%d): cannot create _transformToGps: %s",
            srid, error2));

  }
}

GpsTranslator::~GpsTranslator() {
  delete _transformToGps;
  delete _transformFromGps;
  _transformToGps = nullptr;
  _transformFromGps = nullptr;
}

void GpsTranslator::fromGpsRaw(const GpsCoordinates &input,
    EastNorthCoordinates &output) const {
  if (_epsg > 0) {
    output._east = input._longitude;
    output._north = input._latitude;
  } else if (_esri > 0) {
    output._east = input._longitude;
    output._north = input._latitude;
  }
  _transformFromGps->Transform(1, &output._north, &output._east);
  if (_esri > 0) {
    auto tmp = output._north;
    output._north = output._east;
    output._east = tmp;
  }
}
void GpsTranslator::toGpsRaw(const EastNorthCoordinates &input,
    GpsCoordinates &output) const {
  if (_epsg > 0) {
    output._latitude = input._east;
    output._longitude = input._north;
  } else if (_esri > 0) {
    output._latitude = input._north;
    output._longitude = input._east;
  }
  _transformToGps->Transform(1, &output._longitude, &output._latitude);
  auto error = CPLGetLastErrorMsg();
  if (error != nullptr && *error != '\0') {
    _logger.say(LV_ERROR,
        formatCString("toGpsRaw(): cannot transform(%f/%f): %s", input._east,
            input._north, error));

  } else {
    if (_epsg > 0 || _esri > 0) {
      auto tmp = output._latitude;
      output._latitude = output._longitude;
      output._longitude = tmp;
    }

  }
}

} /* namespace cppknife */
