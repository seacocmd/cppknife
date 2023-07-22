/*
 * GpsTranslator_test.cpp
 *
 *  Created on: 18.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../geo/geo.hpp"
#include "google_test.hpp"
namespace cppknife {

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(GpsTranslatorTest, toGps3035) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 3035);
  GpsCoordinates gps;
  EastNorthCoordinates eastWest(4343266.000, 2737077.000);
  translator.toGpsRaw(eastWest, gps);
  ASSERT_STREQ(
      formatCString("%0.6f, %0.6f", gps._latitude, gps._longitude).c_str(),
      "47.746904, 10.296741");
  delete logger;
}
TEST(GpsTranslatorTest, fromGps3035) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 3035);
  GpsCoordinates gps(47.746903997782887, 10.296741463186294);
  EastNorthCoordinates eastWest;
  translator.fromGpsRaw(gps, eastWest);
  ASSERT_STREQ(
      formatCString("%0.3f, %0.3f", eastWest._east, eastWest._north).c_str(),
      "4343266.000, 2737077.000");
  delete logger;
}
TEST(GpsTranslatorTest, toGps102004) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 102004);
  GpsCoordinates gps;
  // New York
  // SRID 102004: 1847413.89, 408620.67
  // GPS: 40.8465043,-73.3517423
  EastNorthCoordinates eastWest(1847413.89, 408620.67);
  translator.toGpsRaw(eastWest, gps);
  ASSERT_STREQ(
      formatCString("%0.6f, %0.6f", gps._latitude, gps._longitude).c_str(),
      "40.648988, -73.826685");
  delete logger;
}
TEST(GpsTranslatorTest, fromGps102004) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 102004);
  // New York
  // SRID 102004: 1847413.89, 408620.67
  // GPS: 40.8465043,-73.3517423
  GpsCoordinates gps(40.8465043, -73.3517423);
  EastNorthCoordinates eastWest;
  translator.fromGpsRaw(gps, eastWest);
  ASSERT_STREQ(
      formatCString("%0.3f, %0.3f", eastWest._east, eastWest._north).c_str(),
      "1880788.008, 439526.455");
  delete logger;
}

TEST(GpsTranslatorTest, toGps102024) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 102024);
  GpsCoordinates gps;
  // Kapstadt
  // SRID 102004: -670321, -3690487
  // GPS: -33.930198874979645,18.42362037986
  EastNorthCoordinates eastWest(-670321, -3690487);
  translator.toGpsRaw(eastWest, gps);
  ASSERT_STREQ(
      formatCString("%0.6f, %0.6f", gps._latitude, gps._longitude).c_str(),
      "-33.930199, 18.423620");
  delete logger;
}
TEST(GpsTranslatorTest, fromGps102024) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 102024);
  // Kapstadt
  // SRID 102004: -670321, -3690487
  // GPS: -33.930198874979645,18.42362037986
  GpsCoordinates gps(-33.93019887, 18.42362037);
  EastNorthCoordinates eastWest;
  translator.fromGpsRaw(gps, eastWest);
  ASSERT_STREQ(
      formatCString("%0.3f, %0.3f", eastWest._east, eastWest._north).c_str(),
      "-670321.001, -3690486.999");
  delete logger;
}

/*
 *     def testGpsTranslator100mESRI102030(self):
 if inDebug():
 return
 translator = shapetool.GpsTranslator(102030, 'vs4')
 # Mumbai
 # near 18.9709357,72.8003199
 lat, long = translator.toGpsRaw(-6003025.1, 3112734.4)
 self.assertEqual(18.966056687693012, lat)
 self.assertEqual(72.82218531642943, long)
 *
 */
TEST(GpsTranslatorTest, toGps102030) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 102030);
  GpsCoordinates gps;
  // Mumbai
  // SRID 102030: -6003025.1, 3112734.4
  // GPS: 18.9709357,72.8003199
  EastNorthCoordinates eastWest(-6003025.1, 3112734.4);
  translator.toGpsRaw(eastWest, gps);
  ASSERT_STREQ(
      formatCString("%0.6f, %0.6f", gps._latitude, gps._longitude).c_str(),
      "18.966057, 72.822185");
  delete logger;
}
TEST(GpsTranslatorTest, fromGps102030) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 102030);
  // Mumbai
  // SRID 102030: -6003025.1, 3112734.4
  // GPS: 18.9709357,72.8003199
  GpsCoordinates gps(18.9709357, 72.8003199);
  EastNorthCoordinates eastWest;
  translator.fromGpsRaw(gps, eastWest);
  ASSERT_STREQ(
      formatCString("%0.3f, %0.3f", eastWest._east, eastWest._north).c_str(),
      "-6005625.400, 3112810.202");
  delete logger;
}

TEST(GpsTranslatorTest, duration) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  GpsTranslator translator(*logger, 3035);
  GpsCoordinates gps(47.746903997782887, 10.296741463186294);
  EastNorthCoordinates eastWest;
  double start = nowAsDouble();
  for (int ix = 0; ix < 100000; ix++) {
    translator.fromGpsRaw(gps, eastWest);
    if ((gps._latitude += 0.01) > 89) {
      gps._latitude = 8.3;
    }
    if ((gps._longitude += 0.02) > 89) {
      gps._longitude = 7.2;
    }
  }
  logger->say(LV_INFO,
      timeDifferenceToString(nowAsDouble() - start,
          "= runtime fromGps(100 000): %hh%mm%s.%3s"));
  start = nowAsDouble();
  for (int ix = 0; ix < 100000; ix++) {
    translator.toGpsRaw(eastWest, gps);
    if ((eastWest._east += 7.3) > 5100200.0) {
      eastWest._east = 2100100.3;
    }
    if ((eastWest._north += 2.02) > 6100200.0) {
      eastWest._north = 1100100.2;
    }
  }
  logger->say(LV_INFO,
      timeDifferenceToString(nowAsDouble() - start,
          "= runtime toGps(10 000): %hh%mm%s.%3s"));
}

}

