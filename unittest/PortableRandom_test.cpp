/*
 * PortableRandom_test.cpp
 *
 *  Created on: 03.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

using namespace cppknife;
TEST(PortableRandomTest, basics) {
  FEW_TESTS;
  PortableRandom random1;
  ASSERT_EQ(random1.nextInt(10), 9);
  ASSERT_EQ(random1.nextInt(0x7ffffffe), 1454762095);
}
TEST(PortableRandomTest, nextInt) {
  FEW_TESTS;
  PortableRandom random1;
  random1.setSeed(1, 2, 3);
  ASSERT_EQ(random1.nextInt(10), 6);
  ASSERT_EQ(random1.nextInt(-3, 33), 15);
  int lowerBounds = 0;
  int upperBounds = 0;
  for (int ix = 0; ix < 5000; ix++) {
    auto value = random1.nextInt(-10, 10);
    if (value == -10) {
      lowerBounds++;
    }
    if (value == 10) {
      upperBounds++;
    }
    ASSERT_TRUE(value >= -10);
    if (value > 10) {
      FAIL();
    }
  }
  ASSERT_TRUE(upperBounds >= 100);
  ASSERT_TRUE(lowerBounds >= 200);
}
TEST(PortableRandomTest, nextFloat) {
  FEW_TESTS;
  PortableRandom random1;
  double sum = 0.0;
  int uBounds = 0;
  int lBounds = 0;
  for (int ix = 0; ix < 5000; ix++) {
    auto value = random1.nextFloat();
    if (value == 1.0) {
      uBounds++;
    }
    if (value == 0.0) {
      lBounds++;
    }
    ASSERT_TRUE(value <= 1.0);
    ASSERT_TRUE(value >= 0.0);
    sum += value;
  }
  ASSERT_EQ(2505.0472400824824, sum);
}
TEST(PortableRandomTest, setPhrase) {
  FEW_TESTS;
  PortableRandom random1;
  random1.setPhrase("Hello");
  ASSERT_EQ(639860484, random1.nextInt());
  random1.setPhrase("Hello", "world");
  ASSERT_EQ(440277348, random1.nextInt());
  random1.setPhrase("Hello", "world", "!");
  ASSERT_EQ(1093851961, random1.nextInt());
}
TEST(PortableRandomTest, distribution) {
  FEW_TESTS;
  PortableRandom random;
  BaseRandom::testDistribution50(random, 10000);
  BaseRandom::testDistributionInt(random, 1000, 10000);
}
