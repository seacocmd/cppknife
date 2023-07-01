/*
 * Rectangle_test.cpp
 *
 *  Created on: 04.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "google_test.hpp"
#include "../geo/geo.hpp"
using namespace cppknife;
TEST(RectangleTest, asString) {
  Rectangle r1(2.1, -3.2, 4.0, 0.5);
  ASSERT_EQ(r1.asString(), "[2.10,-3.20+4.00+0.50]");
  ASSERT_EQ(r1.asPointString(1), "[2.1,-3.2/6.1,-2.7]");
  Rectangle r2(Point2D(-4.3, 1.1), 2.1, 1.9);
  ASSERT_EQ(r2.asString(1), "[-4.3,1.1+2.1+1.9]");
  auto r3 = r1;
  ASSERT_EQ(r3.asString(), "[2.10,-3.20+4.00+0.50]");
}

TEST(RectangleTest, contains) {
  auto tolerance = toleranceOf(2.1);
  auto eps = 0.1;
  Rectangle r1(2.1, -3.2, 4.0, 0.5);

  ASSERT_TRUE(r1.contains(2.1, -3.2, tolerance));
  ASSERT_TRUE(r1.contains(2.1 + eps, -3.2, tolerance));
  ASSERT_TRUE(r1.contains(2.1, -3.2 + eps, tolerance));
  ASSERT_TRUE(
      r1.contains(2.1 - 0.1 * tolerance, -3.2 + 0.1 * tolerance, tolerance));
  ASSERT_TRUE(r1.contains(2.1 + eps, -3.2 + eps, tolerance));
  ASSERT_FALSE(r1.contains(2.1 - eps, -3.2, tolerance));
  ASSERT_FALSE(r1.contains(2.1, -3.2 - eps, tolerance));
}

TEST(RectangleTest, outlineOf) {
  Rectangle r1(2.1, -3.2, 4.0, 0.5);
  Rectangle r2;
  ASSERT_EQ(r1.outlineOf(r2).asString(), "[2.10,-3.20+4.00+0.50]");
  ASSERT_TRUE(r1.isEqual(r2, 1E-14));
}

