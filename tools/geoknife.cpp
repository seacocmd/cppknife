/*
 * geoknife.cpp
 *
 *  Created on: 21.08.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "dbknife.hpp"
#include "../os/os.hpp"
#include "../db/db.hpp"
#include "tools.hpp"

namespace cppknife {

bool geoknifeUnderTest = false;

void examples() {
  printf(
      R"""(# Translate two coordinates of the CRS EPSG 3035 into GPS:
geoknife to-gps --srid=3035 "407322.7 2732199.5"

# Translate GPS coordinates (lat long) into ESRI 102030:
geoknife from-gps --srid=102030 "47.993827 -12.30272390"

# Describe the usage:
geoknife --help
)""");
}


/**
 * Manages the "to-gps" sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 * @return 0: success Otherwise: the exit code.
 */
int toGps(ArgumentParser &parser, Logger &logger) {
  int rc = 9;
  return rc;
}

int geoknife(int argc, char **argv, Logger *loggerExtern) {
  auto logger =
      loggerExtern == nullptr ?
          buildMemoryLogger(200, LV_SUMMARY) : loggerExtern;
  double start = nowAsDouble();
  int rc = 0;
  ArgumentParser parser("geoknife", logger, "Database management and more");
  parser.add("--log-level", "-l", DT_NAT,
      "Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG",
      "5");
  parser.add("--verbose", "-v", DT_BOOL, "Show more information");
  parser.add("--examples", nullptr, DT_BOOL, "Show usage examples", "false");
  parser.addMode("mode", "What should be done:", "to-gps,from-gps");
  ArgumentParser toGpsParser("to-gps", logger,
      "Translate a coordinate pair to GPS");
  parser.addSubParser("mode", "to-gps", toGpsParser);
  toGpsParser.add("--srid", "-S", DT_NAT,
      "The identifier of the coordinate system (see POSTGIS).", "3035", "3035");
  toGpsParser.add("coordinate", nullptr, DT_STRING,
      "The coordinates: the first two floating numbers will be taken as east-north coordinates",
      nullptr, "47329882.7 27339487.2|east: 47329882.7 north: 27339487.2");
  ArgumentParser fromGpsParser("from-gps", logger,
      "Translate a coordinate pair to GPS");
  parser.addSubParser("mode", "from-gps", fromGpsParser);
  fromGpsParser.add("--srid", "-S", DT_NAT,
      "The identifier of the coordinate system (see POSTGIS).", "3035", "3035");
  fromGpsParser.add("coordinate", nullptr, DT_STRING,
      "The coordinates: the first two floating numbers will be taken as latitude longitude",
      nullptr, "47.928392 11.3829283|@47.928392,11.3829283,");
  auto verbose = parser.asBool("verbose");
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
  } else if (parser.asBool("examples")) {
    examples();
  } else {
    auto level = static_cast<LogLevel>(parser.asInt("log-level", LV_SUMMARY));
    logger->setLevel(level);
    try {
      if (parser.isMode("mode", "to-gps")) {
        rc = toGps(parser, *logger);
      } else {
        printf("%s\n", parser.usage("unknown mode", nullptr, false).c_str());
      }
    } catch (ArgumentException &e) {
      logger->say(LV_ERROR, e.message());
    }
    if (verbose) {
      logger->say(LV_SUMMARY,
          timeDifferenceToString(nowAsDouble() - start,
              "= runtime: %hh%mm%s.%3s"));
    }
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}

} /* namespace */
