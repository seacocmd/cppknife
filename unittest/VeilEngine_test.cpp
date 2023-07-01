/*
 * VeilEngine_test.cpp
 *
 *  Created on: 17.06.2022
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

TEST(VeilEngineTest, veil) {
  FEW_TESTS;
  auto config = temporaryFile("veilengine.conf", "unittest", true);

  auto logger = buildMemoryLogger(10, LV_FINEST);
  VeilEngine::buildConfiguration(config.c_str(), "rambo",
      "unittest", "HumbaHumba", "ForgetIt", *logger);
  VeilEngine engine("unittest", *logger, config.c_str());
  engine.reset();
  auto encrypted = engine.veil("Morning has broken!");
  ASSERT_EQ(encrypted.size(), 19 + 4);
  delete logger;
}
TEST(VeilEngineTest, unveil) {
  FEW_TESTS;
  auto config = temporaryFile("veilengine.conf", "unittest", true);

  auto logger = buildMemoryLogger(10, LV_FINEST);
  VeilEngine::buildConfiguration(config.c_str(), "rambo", "unittest",
      "HumbaHumba", "ForgetIt", *logger);
  VeilEngine engine("unittest", *logger, config.c_str());
  engine.reset();
  const char *text = "Morning has broken! !$%&/()=?";
  auto encrypted = engine.veil(text);
  engine.reset();
  auto decrypted = engine.unveil(encrypted.c_str());
  ASSERT_STREQ(decrypted.c_str(), text);
  delete logger;
}

TEST(VeilEngineTest, verify) {
  FEW_TESTS;
  auto config = temporaryFile("veilengine.conf", "unittest", true);

  auto logger = buildMemoryLogger(10, LV_FINEST);
  VeilEngine::buildConfiguration(config.c_str(), "rambo", "unittest",
      "HumbaHumba", "ForgetIt", *logger);
  VeilEngine engine("unittest", *logger, config.c_str());
  ASSERT_TRUE(engine.verify("unittest", "?"));
  delete logger;
}

