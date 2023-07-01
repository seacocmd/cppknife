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
  parser.addMode("mode", "What should be done:", "to-gps,from-gps");
  ArgumentParser toGpsParser("to-gps", logger,
      "Translate a coordinate pair to GPS");
  parser.addSubParser("mode", "to-gps", toGpsParser);
  toGpsParser.add("--srid", "-S", DT_NAT,
      "The identifier of the coordinate system (see POSTGIS).", "3035", "3035");

  auto verbose = parser.asBool("verbose");
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
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
  }
  if (verbose) {
    logger->say(LV_SUMMARY,
        timeDifferenceToString(nowAsDouble() - start,
            "= runtime: %hh%mm%s.%3s"));
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}

} /* namespace */
