/*
 * Rectangle_test.cpp
 *
 *  Created on: 03.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "google_test.hpp"
#include "../geo/geo.hpp"
using namespace cppknife;
TEST(Point2DTest, asString) {
  Point2D p1;
  ASSERT_EQ(p1.asString(0), "[0,0]");
  Point2D p2(1.5, -2.3);
  ASSERT_EQ(p2.asString(1), "[1.5,-2.3]");
  Point2D p3(p2);
  ASSERT_EQ(p3.asString(), "[1.50,-2.30]");
  Point2D p4 = p2;
  ASSERT_EQ(p4.asString(), "[1.50,-2.30]");
  float list[] = { 3.2, -4.4 };
  Point2D *p5 = Point2D::fromList(list);
  ASSERT_EQ(p5->asString(), "[3.20,-4.40]");
  delete p5;
}

TEST(Point2DTest, equals) {
  Point2D p1(1.5, -2.3);
  Point2D p2(1.5, -2.3);
  Point2D p3(1.6, -2.3);
  Point2D p4(1.5, -2.4);
  ASSERT_TRUE(p1 == p2);
  ASSERT_TRUE(p1 != p3);
  ASSERT_TRUE(p1 != p4);
}
TEST(Point2DTest, outlineOf) {
  Point2D p1(1.5, -2.3);
  Rectangle r1;
  auto r2 = p1.outlineOf(r1);
  ASSERT_EQ(r2._point._x, 1.5);
  ASSERT_EQ(r2._point._y, -2.3);
  ASSERT_EQ(r2._width, 0.0);
  ASSERT_EQ(r2._height, 0.0);
}

TEST(Point2DTest, isEqual) {
  auto eps = 1E-15;
  auto tolerance = 1E-14;
  Point2D p1(1.5, -2.3);
  Point2D p2(1.5 + eps, -2.3);
  Point2D p3(1.5, -2.3 - eps);
  Point2D p4(1.5 + tolerance + eps, -2.3 - eps);
  Point2D p5(1.5, -2.3 - eps - tolerance - eps);

  ASSERT_TRUE(p1.isEqual(p2, tolerance));
  ASSERT_TRUE(p1.isEqual(p3, tolerance));
  ASSERT_TRUE(p2.isEqual(p3, tolerance));
  ASSERT_FALSE(p1.isEqual(p4, tolerance));
  ASSERT_FALSE(p1.isEqual(p5, tolerance));
}

