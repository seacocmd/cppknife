/*
 * Configuration_test.cpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../text/text.hpp"

using namespace cppknife;

static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

TEST(ConfigurationTest, basics) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn = theOsInfo._tempDirectorySeparator + "configuration.test.txt";

  writeText(fn.c_str(), R"(
# comment
      ; comment2
      string = Who are you?
      nat=12237
      int=-332
      bool.True = True
      bool.False = FALSE
  )");

  SimpleConfiguration config(fn.c_str());
  ASSERT_STREQ("Who are you?", config.asString("string"));
  ASSERT_EQ(12237, config.asNat("nat"));
  ASSERT_EQ(-332, config.asInt("int"));
  ASSERT_EQ(1, config.asBool("bool.True"));
  ASSERT_EQ(0, config.asBool("bool.False"));
  ASSERT_EQ(-1, config.asBool("bool.Unknown"));
  auto names = config.names();
  ASSERT_EQ(5, names.size());
  ASSERT_TRUE(std::find(names.begin(), names.end(), "bool.True") != names.end());
  auto names2 = config.names("^bool.");
  ASSERT_EQ(2, names2.size());
  ASSERT_TRUE(std::find(names2.begin(), names2.end(), "bool.True") != names2.end());
}

TEST(ConfigurationTest, populate) {
  auto logger = buildMemoryLogger();
  Configuration config(logger);
  config.populate(R"""(# comment
[dummy]
number=123
string = abc
)""");
  ASSERT_EQ(123, config.asNat("number"));
  ASSERT_STREQ("abc", config.asString("string"));
  ASSERT_EQ(2, config.names().size());
  delete logger;
}

