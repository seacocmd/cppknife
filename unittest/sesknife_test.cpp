/*
 * Script_test.cpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../tools/sesknife.hpp"
using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(SesKnifeTest, log) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnScript = temporaryFile("example.script", "unittest", true);
  auto fnData = temporaryFile("example.data", "unittest", true);
  std::string script(
      R"""(
log! "Hi !(name)"
log! "File: !(_file)"
log !_main
)""");
  writeText(fnScript.c_str(), script.c_str(), script.size(), logger);
  std::string data(R"""(
Hi World!
)""");
  writeText(fnData.c_str(), data.c_str(), data.size(), logger);
  const char *argv[] = { "-Dname=Jonny", "--trace", fnScript.c_str(),
      fnData.c_str() };
  size_t argc = sizeof argv / sizeof argv[0];
  searchEngineScriptKnife(argc, const_cast<char**>(argv), logger);
  delete logger;
}

