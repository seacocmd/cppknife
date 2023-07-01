/*
 * GeoHelper_test.cpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "google_test.hpp"
#include "../geo/geo.hpp"

using namespace cppknife;
TEST(HelperTest, betweenFloats) {
  auto tolerance = 1E-10;
  auto little = 1E-9;
  auto eps = 0.5 * tolerance;

  ASSERT_TRUE(betweenFloats(0, 0.5, 1, tolerance));
  ASSERT_TRUE(betweenFloats(0, 1, 1, tolerance));
  ASSERT_TRUE(betweenFloats(0, 0, 1, tolerance));
  ASSERT_TRUE(betweenFloats(0, 0 + eps, 1, tolerance));
  ASSERT_TRUE(betweenFloats(0, 0 - eps, 1, tolerance));
  ASSERT_TRUE(betweenFloats(0, 1 + eps, 1, tolerance));
  ASSERT_TRUE(betweenFloats(0, 1 - eps, 1, tolerance));
  ASSERT_FALSE(betweenFloats(0, 0 - little, 1, tolerance));
  ASSERT_FALSE(betweenFloats(0, 1 + little, 1, tolerance));
}
TEST(HelperTest, betweenFloatsExclusive) {
  auto tolerance = 1E-10;
  auto little = 1E-9;
  auto eps = 0.5 * tolerance;

  ASSERT_TRUE(betweenFloatsExclusive(0, 0.5, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 1, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 0, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 0 + eps, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 0 - eps, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 1 + eps, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 1 - eps, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 0 - little, 1, tolerance));
  ASSERT_FALSE(betweenFloatsExclusive(0, 1 + little, 1, tolerance));
}
TEST(HelperTest, equalFloats) {
  auto tolerance = 1E-10;
  auto little = 1E-9;
  auto eps = 0.5 * tolerance;

  ASSERT_FALSE(equalFloats(0, 1, tolerance));
  ASSERT_TRUE(equalFloats(0, 0, tolerance));
  ASSERT_TRUE(equalFloats(-2.33, -2.33, tolerance));
  ASSERT_TRUE(equalFloats(-2.33 + eps, -2.33, tolerance));
  ASSERT_TRUE(equalFloats(-2.33, -2.33 - eps, tolerance));
  ASSERT_FALSE(equalFloats(-2.33 + little, -2.33, tolerance));
  ASSERT_FALSE(equalFloats(-2.33, -2.33 - little, tolerance));
}
TEST(HelperTest, lowerEqualFloats) {
  auto tolerance = 1E-10;
  auto little = 1E-9;
  auto eps = 0.5 * tolerance;

  ASSERT_FALSE(lowerEqualFloats(1, 0, tolerance));
  ASSERT_TRUE(lowerEqualFloats(0, 1, tolerance));
  ASSERT_TRUE(lowerEqualFloats(0, 0, tolerance));
  ASSERT_TRUE(lowerEqualFloats(-2.33, -2.33, tolerance));
  ASSERT_TRUE(lowerEqualFloats(-2.33 + eps, -2.33, tolerance));
  ASSERT_TRUE(lowerEqualFloats(-2.33, -2.33 - eps, tolerance));
  ASSERT_FALSE(lowerEqualFloats(-2.33 + little, -2.33, tolerance));
  ASSERT_TRUE(lowerEqualFloats(-2.33 - little, -2.33, tolerance));
}
TEST(HelperTest, lowerFloats) {
  auto tolerance = 1E-10;
  auto little = 1E-9;
  auto eps = 0.5 * tolerance;

  ASSERT_FALSE(lowerFloats(1, 0, tolerance));
  ASSERT_TRUE(lowerFloats(0, 1, tolerance));
  ASSERT_FALSE(lowerFloats(0, 0, tolerance));
  ASSERT_FALSE(lowerFloats(-2.33, -2.33, tolerance));
  ASSERT_FALSE(lowerFloats(-2.33 + eps, -2.33, tolerance));
  ASSERT_FALSE(lowerFloats(-2.33, -2.33 - eps, tolerance));
  ASSERT_FALSE(lowerFloats(-2.33 + little, -2.33, tolerance));
  ASSERT_TRUE(lowerFloats(-2.33 - little, -2.33, tolerance));
}
TEST(HelperTest, toleranceOf) {
  ASSERT_EQ(toleranceOf(33), 33 * 1E-14);
}
