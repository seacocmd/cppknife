/*
 * KissRandom_test.cpp
 *
 *  Created on: 08.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS if (onlyFewTests()) return

TEST(KissRandomTest, kissRandomClass) {
  FEW_TESTS;
  KissRandom rand;
  rand.setPhrase("HilliBilly");
  ASSERT_EQ(2136584378, rand.nextInt());
  rand.setPhrase("HilliBilly", "1024$ && more");
  ASSERT_EQ(482555338, rand.nextInt());
  rand.setPhrase("HilliBilly", "1024$ && more", "!");
  ASSERT_EQ(1584961310, rand.nextInt());
}

TEST(KissRandomTest, distribution) {
  FEW_TESTS;
  KissRandom random;
  BaseRandom::testDistribution50(random, 10000);
  BaseRandom::testDistributionInt(random, 1000, 10000);
}
