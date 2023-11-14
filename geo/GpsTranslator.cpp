/*
 * GpsTranslator.cpp
 *
 *  Created on: 18.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

/**
 * Debian package: apt install libgdal-dev
 * see: /usr/include/gdal and /usr/lib/libgdal.a
 */
#include "geo.hpp"
std::map<int, const char*> s_wkt(
    {
        { 4908,
            R"""(GEOCCS["GR96",
DATUM["Greenland_1996",
  SPHEROID["GRS 1980",6378137,298.257222101,
      AUTHORITY["EPSG","7019"]],
  AUTHORITY["EPSG","6747"]],
PRIMEM["Greenwich",0,
  AUTHORITY["EPSG","8901"]],
UNIT["metre",1,
  AUTHORITY["EPSG","9001"]],
AXIS["Geocentric X",OTHER],
AXIS["Geocentric Y",OTHER],
AXIS["Geocentric Z",NORTH],
AUTHORITY["EPSG","4908"]]
)""" }, });

std::map<int, const char*> s_esriData(
    {
        { 102002,
            R"""(PROJCS["Canada_Lambert_Conformal_Conic",
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
PARAMETER["latitude_of_origin",40],
PARAMETER["central_meridian",-96],
PARAMETER["standard_parallel_1",50],
PARAMETER["standard_parallel_2",70],
PARAMETER["false_easting",0],
PARAMETER["false_northing",0],
UNIT["metre",1,
  AUTHORITY["EPSG","9001"]],
AXIS["Easting",EAST],
AXIS["Northing",NORTH],
AUTHORITY["ESRI","102002"]]
)""" },
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
        { 102009,
            R"""(PROJCS["North_America_Lambert_Conformal_Conic",
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
    PARAMETER["latitude_of_origin",40],
    PARAMETER["central_meridian",-96],
    PARAMETER["standard_parallel_1",20],
    PARAMETER["standard_parallel_2",60],
    PARAMETER["false_easting",0],
    PARAMETER["false_northing",0],
    UNIT["metre",1,
        AUTHORITY["EPSG","9001"]],
    AXIS["Easting",EAST],
    AXIS["Northing",NORTH],
    AUTHORITY["ESRI","102009"]]
)""" },
        { 102012,
            R"""(PROJCS["Asia_Lambert_Conformal_Conic",
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
    PARAMETER["central_meridian",105],
    PARAMETER["standard_parallel_1",30],
    PARAMETER["standard_parallel_2",62],
    PARAMETER["false_easting",0],
    PARAMETER["false_northing",0],
    UNIT["metre",1,
        AUTHORITY["EPSG","9001"]],
    AXIS["Easting",EAST],
    AXIS["Northing",NORTH],
    AUTHORITY["ESRI","102012"]]
)""" },
        { 102015,
            R"""(PROJCS["South_America_Lambert_Conformal_Conic",
    GEOGCS["SAD69",
        DATUM["South_American_Datum_1969",
            SPHEROID["GRS 1967 Modified",6378160,298.25,
                AUTHORITY["EPSG","7050"]],
            AUTHORITY["EPSG","6618"]],
        PRIMEM["Greenwich",0,
            AUTHORITY["EPSG","8901"]],
        UNIT["degree",0.0174532925199433,
            AUTHORITY["EPSG","9122"]],
        AUTHORITY["EPSG","4618"]],
    PROJECTION["Lambert_Conformal_Conic_2SP"],
    PARAMETER["latitude_of_origin",-32],
    PARAMETER["central_meridian",-60],
    PARAMETER["standard_parallel_1",-5],
    PARAMETER["standard_parallel_2",-42],
    PARAMETER["false_easting",0],
    PARAMETER["false_northing",0],
    UNIT["metre",1,
        AUTHORITY["EPSG","9001"]],
    AXIS["Easting",EAST],
    AXIS["Northing",NORTH],
    AUTHORITY["ESRI","102015"]]
)""" },
        { 102019,
            R"""(PROJCS["South_Pole_Azimuthal_Equidistant",
    GEOGCS["WGS 84",
        DATUM["WGS_1984",
            SPHEROID["WGS 84",6378137,298.257223563,
                AUTHORITY["EPSG","7030"]],
            AUTHORITY["EPSG","6326"]],
        PRIMEM["Greenwich",0],
        UNIT["Degree",0.0174532925199433]],
    PROJECTION["Azimuthal_Equidistant"],
    PARAMETER["latitude_of_center",-90],
    PARAMETER["longitude_of_center",0],
    PARAMETER["false_easting",0],
    PARAMETER["false_northing",0],
    UNIT["metre",1,
        AUTHORITY["EPSG","9001"]],
    AXIS["Easting",EAST],
    AXIS["Northing",NORTH],
    AUTHORITY["ESRI","102019"]]
)""" },
        { 102020,
            R"""(PROJCS["South_Pole_Lambert_Azimuthal_Equal_Area",
    GEOGCS["GCS_WGS_1984",
        DATUM["D_WGS_1984",
            SPHEROID["WGS_1984",6378137.0,298.257223563]],
        PRIMEM["Greenwich",0.0],
        UNIT["Degree",0.0174532925199433]],
    PROJECTION["Lambert_Azimuthal_Equal_Area"],
    PARAMETER["False_Easting",0.0],
    PARAMETER["False_Northing",0.0],
    PARAMETER["Central_Meridian",0.0],
    PARAMETER["Latitude_Of_Origin",-90.0],
    UNIT["Meter",1.0]]
)""" },
        { 102022,
            R"""(PROJCS["Africa_Albers_Equal_Area_Conic",
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
    PROJECTION["Albers_Conic_Equal_Area"],
    PARAMETER["latitude_of_center",0],
    PARAMETER["longitude_of_center",25],
    PARAMETER["standard_parallel_1",20],
    PARAMETER["standard_parallel_2",-23],
    PARAMETER["false_easting",0],
    PARAMETER["false_northing",0],
    UNIT["metre",1,
        AUTHORITY["EPSG","9001"]],
    AXIS["Easting",EAST],
    AXIS["Northing",NORTH],
    AUTHORITY["ESRI","102022"]])""" },
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
        nullptr), _transformations(0) {
  const char *error1 = nullptr;
  const char *error2 = nullptr;

  if (_epsg == 4908) {
    if (s_wkt.find(srid) != s_wkt.end()) {
      const char *description = s_wkt[srid];
      _other.importFromWkt(&description);
    } else {
      auto msg = formatCString("GpsTranslator: unknown WKT: ", srid);
      _logger.say(LV_ERROR, msg);
    }
  } else if (_epsg > 0) {
    _other.importFromEPSG(_epsg);
  } else if (_esri > 0) {
    auto data = s_esriData;
    if (s_esriData.find(srid) == s_esriData.end()) {
      auto msg = formatCString("GpsTranslator: unknown ESRI: ", srid);
      _logger.say(LV_ERROR, msg);
      throw InternalError(msg);
    }
    const char *esri = s_esriData[_esri];
    _other.SetFromUserInput(esri);
    if (true || _esri > 0) {
      _other.morphFromESRI();
    }
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
    EastNorthCoordinates &output) {
  _transformations++;
  if (_epsg > 0) {
    output._east = input._longitude;
    output._north = input._latitude;
  } else if (_esri > 0) {
    output._east = input._longitude;
    output._north = input._latitude;
  }
  _transformFromGps->Transform(1, &output._north, &output._east);
  if (_esri > 0 || (_epsg != 3035 && _epsg != 6362)) {
    auto tmp = output._north;
    output._north = output._east;
    output._east = tmp;
  }
}
void GpsTranslator::toGpsRaw(const EastNorthCoordinates &input,
    GpsCoordinates &output) {
  _transformations++;
  bool invers = _epsg == 3035 || _epsg == 6263;
  if (invers) {
    output._latitude = input._east;
    output._longitude = input._north;
  } else {
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
