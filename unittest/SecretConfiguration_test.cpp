/*
 * SecretConfiguration_test.cpp
 *
 *  Created on: 17.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../tools/tools.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS if (onlyFewTests()) return
std::string *globalPtr = nullptr;

TEST(SecretConfigurationTest, cleartext) {
  FEW_TESTS;
  auto configFile = temporaryFile("secretconfig.conf", "unittest", true);
  globalPtr = &configFile;
  writeText(configFile.c_str(), R"""(# test configuration
a=a string
int=-3
nat=33
double=4.77
bool=True
)""");
  auto logger = buildMemoryLogger(10, LV_FINEST);
  SecretConfiguration config(configFile.c_str(), "unittest", *logger);
  ASSERT_STREQ(config.asString("a"), "a string");
  ASSERT_EQ(config.asInt("int"), -3);
  ASSERT_EQ(config.asNat("nat"), 33);
  //ASSERT_EQ(config.asFloat("double"), 4.77);
  ASSERT_EQ(config.asBool("bool"), true);
  SecretConfiguration config2(configFile.c_str(), "unittest", *logger);
  ASSERT_STREQ(config2.asString("a"), "a string");
  ASSERT_EQ(config2.asInt("int"), -3);
  ASSERT_EQ(config2.asNat("nat"), 33);
  //ASSERT_EQ(config.asFloat("double"), 4.77);
  ASSERT_EQ(config2.asBool("bool"), true);

  delete logger;
}
