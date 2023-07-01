/*
 * Process_test.cpp
 *
 *  Created on: 18.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../os/os.hpp"
using namespace cppknife;

using namespace cppknife;

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS if (onlyFewTests()) return

TEST(ProcessTest, executeToFile) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DETAIL);
  auto tmpFile = temporaryFile("execute.txt", "unittest", true);
  Process proc(logger);
  std::string out;
  const char *argv[] = {"/bin/ewc", nullptr};
  unlink(tmpFile.c_str());
  ASSERT_EQ(0,
      proc.execute(argv, "Hello world!", nullptr, nullptr, tmpFile.c_str()));
  auto content = readAsString(tmpFile.c_str(), logger);
  ASSERT_STREQ("12", content.c_str());
  delete logger;
}

TEST(ProcessTest, executeToString) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DETAIL);
  Process proc(logger);
  std::string out;
  const char *argv[] = {"/bin/ewc", nullptr};
  ASSERT_EQ(0, proc.execute(argv, "Hello world!", nullptr, &out));
  ASSERT_STREQ("12\n", out.c_str());
  delete logger;
}

TEST(ProcessTest, executeAndPipeToFile) {
  //FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DETAIL);
  auto tmpFile = temporaryFile("execute.txt", "unittest", true);
  auto tmpInput = temporaryFile("execute.input", "unittest", true);
  Process proc(logger);
  std::string out;
  const char *argv1[] = { "/bin/ecat", nullptr };
  const char *argv2[] = { "/usr/bin/wc", "-w", nullptr };
  ASSERT_EQ(0,
      proc.executeAndPipe(argv1, argv2, nullptr, tmpInput.c_str(), nullptr,
          tmpFile.c_str()));
  auto content = readAsString(tmpFile.c_str(), logger);
  ASSERT_STREQ("Hi", content.c_str());
  delete logger;
}

