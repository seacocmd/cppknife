/*
 * BaseRandom_test.cpp
 *
 *  Created on: 02.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS if (onlyFewTests()) return

using namespace cppknife;
TEST(BaseRandomTest, basics) {
  FEW_TESTS;
  BaseRandom random1;
  random1.setSeed(0x12345678);
  ASSERT_EQ(random1.nextInt(10), 3);
  ASSERT_EQ(random1.nextInt(-3, 33), 3);
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
  ASSERT_TRUE(upperBounds >= 200);
  ASSERT_TRUE(lowerBounds >= 200);
}
TEST(BaseRandomTest, nextBool) {
  FEW_TESTS;
  BaseRandom random1;
  int countTrue = 0;
  int countFalse = 0;
  for (int ix = 0; ix < 5000; ix++) {
    if (random1.nextBool()) {
      countTrue++;
    } else {
      countFalse++;
    }
  }
  ASSERT_EQ(2511, countFalse);
  ASSERT_EQ(2500 - 11, countTrue);
}
TEST(BaseRandomTest, nextFloat) {
  FEW_TESTS;
  BaseRandom random1;
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
  ASSERT_EQ(2487.8090766992495, sum);
}
TEST(BaseRandomTest, setPhrase) {
  FEW_TESTS;
  BaseRandom random1;
  random1.setPhrase("Hello");
  ASSERT_EQ(1358070500, random1.nextInt());
  random1.setPhrase("Hello", "world");
  ASSERT_EQ(684852966, random1.nextInt());
  random1.setPhrase("Hello", "world", "!");
  ASSERT_EQ(1033665667, random1.nextInt());
}
TEST(BaseRandomTest, nextInt50) {
  FEW_TESTS;
  BaseRandom random1;
  int64_t sum = 0;
  for (int ix = 0; ix < 5000; ix++) {
    auto value = random1.nextInt50();
    if (value > 1125899906842624LL) {
      FAIL();
    }
    ASSERT_TRUE(value >= 0);
    sum += value;
  }
  ASSERT_EQ(2830787413184152494, sum);
}
TEST(BaseRandomTest, distribution) {
  FEW_TESTS;
  BaseRandom random;
  BaseRandom::testDistribution50(random, 10000);
  BaseRandom::testDistributionInt(random, 200, 10000);
}
TEST(BaseRandomTest, hashText) {
  FEW_TESTS;
  const char *text1 = "1";
  ASSERT_EQ(0x7eadbf20, BaseRandom::hashString(text1));
  ASSERT_EQ(0x7eadbf20, BaseRandom::hashString(text1, strlen(text1)));
  text1 = "11";
  ASSERT_EQ(0x7ec42acf, BaseRandom::hashString(text1));
  ASSERT_EQ(0x7ec42acf, BaseRandom::hashString(text1, strlen(text1)));
  text1 = "111";
  ASSERT_EQ(0x7f656b00, BaseRandom::hashString(text1));
  ASSERT_EQ(0x7f656b00, BaseRandom::hashString(text1, strlen(text1)));
  text1 = "1111";
  ASSERT_EQ(0x301efb30, BaseRandom::hashString(text1));
  ASSERT_EQ(0x301efb30, BaseRandom::hashString(text1, strlen(text1)));
  const char *text2 = "A long line with 2 lines\nand umlauts äöüß";
  ASSERT_EQ(0x6a9654f3, BaseRandom::hashString(text2));
  ASSERT_EQ(0x6a9654f3, BaseRandom::hashString(text2, strlen(text2)));
}
