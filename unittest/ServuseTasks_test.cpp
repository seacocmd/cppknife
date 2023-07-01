/*
 * ServuseTask_test.cpp
 *
 *  Created on: 14.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../os/os.hpp"
#include "../tools/dbknife.hpp"
#include "../unittest/google_test.hpp"

namespace cppknife {

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(ServuseTestDaemon_test, basic) {
  FEW_TESTS();
  auto fnSource = temporaryFile("veil.txt", "unittest", true);
  auto fnTarget = temporaryFile("veil.data", "unittest", true);
  writeText(fnSource.c_str(), R"""(Hello!
Line2 is very large!
Last line)""");
  const char *argv[] = { "-l5", "veil", fnSource.c_str(), fnTarget.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  auto lines2 = readAsString(fnTarget.c_str(), logger);
  ASSERT_EQ(49, lines2.size());
  ASSERT_EQ(2, countCString(lines2.c_str(), lines2.size(), "\n", 1));
  delete logger;
}

}
/* namespace cppknife */
