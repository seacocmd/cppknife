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
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

#ifndef TEST
define TEST(C,m) void C::m()
#endif

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
#ifdef NEVER
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
#endif

TEST(GpsTranslatorTest, esri102009) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 102009);
// Salt Lake City, Flughafen:
// GPS: 40.7873653,-112.0953466
// Esri102009: -1260161.6  197643.4

GpsCoordinates gps(40.7873653, -112.0953466);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "-1268831.79, 199557.44");
gps.clear();
eastWest.set(-1268831.79, 199557.44);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "40.78736528, -112.09534655");
delete logger;
}
TEST(GpsTranslatorTest, esri102012) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 102012);
// Irkutsk, Flughafen:
// GPS: 52.264391,104.3971971
// Esri102012: -41638.874571 6109770.664311

GpsCoordinates gps(52.264391, 104.3971971);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "-39734.08, 6109181.76");
gps.clear();
eastWest.set(-39734.08, 6109181.76);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "52.26439096, 104.39719705");
delete logger;
}

TEST(GpsTranslatorTest, esri102015) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 102015);
// Cordoba, Flughafen
// GPS: -31.3178438, -64.1648611
// Esri102015: 315047.336703, 1017707.99

GpsCoordinates gps(-31.3178438, -64.1648611);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "-379067.57, 66855.73");
gps.clear();
eastWest.set(-379067.57, 66855.73);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "-31.31784381, -64.16486113");
delete logger;
}

TEST(GpsTranslatorTest, esri102024) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 102024);
// Africa: Lagos, Airport
// GPS: 6.5753322, 3.3147344
// Esri102024: -2254698.62, 668116.97

GpsCoordinates gps(6.5753322, 3.3147344);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "-2254698.62, 668116.98");
gps.clear();
eastWest.set(-2254698.62, 668116.98);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "6.57533220, 3.31473444");
delete logger;
}

TEST(GpsTranslatorTest, esri102019) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 102019);
// Südpol: Hobart City, Tasmania
// GPS: -42.293564, 143.085938
// Esri102019: 3193910.78, -4251716.02

GpsCoordinates gps(-42.293564, 143.085938);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "3193910.77, -4251716.07");
gps.clear();
eastWest.set(3193910.77, -4251716.07);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "-42.29356398, 143.08593796");
delete logger;
}

TEST(GpsTranslatorTest, esri102030) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 102030);
// Southern Asia: Bankok, Palace
// GPS: 13.7061222, 100.3981079
// Esri102030: -2786180.15, 2961753.93

GpsCoordinates gps(13.7061222, 100.3981079);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "-2786180.15, 2961753.94");
gps.clear();
eastWest.set(-2786180.15, 2961753.94);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "13.70612222, 100.39810791");
delete logger;
}

TEST(GpsTranslatorTest, epsg3035) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 3035);
// Europa: Munich, Marienplatz
// GPS: 48.1373968, 11.5728682
// Epsg3035: 4438111.63, 2781673.83

GpsCoordinates gps(48.1373968, 11.5728682);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "4438111.64, 2781673.84");
gps.clear();
eastWest.set(4438111.63, 2781673.83);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "48.13739674, 11.57286810");
delete logger;
}

TEST(GpsTranslatorTest, epsg3574) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 3574);
// Spitzbergen Pyramiden:
// GPS: 78.2253904,15.4755299
// Epsg3574: 1081475.83, -743958.00

GpsCoordinates gps(78.2253904, 15.4755299);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "1081475.83, -743958.00");
gps.clear();
eastWest.set(1081475.83, -743958.00);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "78.22539044, 15.47552981");
delete logger;
}

TEST(GpsTranslatorTest, epsg3832) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 3832);
// Pacific: Jakarta, Monument
// GPS: -6.1957069, 106.8350126
// Epsg3832:-4805104.41, -686442.76

GpsCoordinates gps(-6.1957069, 106.8350126);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "-4805104.42, -686442.77");
gps.clear();
eastWest.set(-4805104.42, -686442.77);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "-6.19570692, 106.83501258");
delete logger;
}

TEST(GpsTranslatorTest, epsg3112) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 3112);
// Australien: Sidney, Opera
// GPS: -33.8567799, 151.2127164
// Epsg3112: 1579489.00, -3921658.79

GpsCoordinates gps(-33.8567799, 151.2127218);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "1579489.00, -3921658.79");
gps.clear();
eastWest.set(1579489.00, -3921658.79);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "-33.85677990, 151.21272184");
delete logger;
}

TEST(GpsTranslatorTest, epsg7755) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 7755);
// Indien: Mumbai, Airport
// GPS: 19.0938967,72.8432949
// Epsg7755: 3854000.99, 3795042.66

GpsCoordinates gps(19.0938967, 72.8432949);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "3259198.21, 3485525.95");
gps.clear();
eastWest.set(3259198.21, 3485525.95);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "19.09389669, 72.84329489");
delete logger;
}

TEST(GpsTranslatorTest, epsg3415) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 3415);
// South China Sea: Paracel Islands
// GPS: 16.6700394,112.7399885
// Epsg3415: 365374.42, 21443.02

GpsCoordinates gps(16.6700394, 112.7399885);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "365374.42, 21443.03");
gps.clear();
eastWest.set(365374.42, 21443.03);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "16.67003942, 112.73998849");
delete logger;
}

TEST(GpsTranslatorTest, epsg6362) {
FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 6362);
// Mexico: Popocatépetl
// GPS: 19.0224419, -98.6382225
// Epsg6362: 2853041.60, 784913.75

GpsCoordinates gps(19.0224419, -98.6382225);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "784913.75, 2853041.60");
gps.clear();
eastWest.set(784913.75, 2853041.60);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "19.02244190, -98.63822254");
delete logger;
}

TEST(GpsTranslatorTest, epsg4908) {
//FEW_TESTS();
auto logger = buildMemoryLogger(100, LV_DEBUG);
GpsTranslator translator(*logger, 4908);
// Greenland, Reykjavík, Harbour
// GPS: 64.1321781, -21.9367912
// Epsg4908: 2588301.54, -1042421.65

GpsCoordinates gps(64.1321781, -21.9367912);
EastNorthCoordinates eastWest;
translator.fromGpsRaw(gps, eastWest);
ASSERT_STREQ(
    formatCString("%0.2f, %0.2f", eastWest._east, eastWest._north).c_str(),
    "2588301.54, -1042421.65");
gps.clear();
eastWest.set(2588301.54, -1042421.65);
translator.toGpsRaw(eastWest, gps);

ASSERT_STREQ(
    formatCString("%0.8f, %0.8f", gps._latitude, gps._longitude).c_str(),
    "64.1321781, -21.93679116");
delete logger;
}

}
/* namespace */

