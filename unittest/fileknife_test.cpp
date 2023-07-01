/*
 * fileknife_test.cpp
 *
 *  Created on: 09.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../tools/fileknife.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

std::string initTree() {
  auto base =
      buildFileTree(
          R"""(/tmp/unittest/dir1/file0.dat;449
/tmp/unittest/dir1/file1.txt;80
/tmp/unittest/dir1/dir2/file2.txt;244
/tmp/unittest/dir1/dir2/dir3/file3.dat;1024
)""");
  base = base.substr(0, base.size() - 1);
  return base;
}

std::string initTreeWc() {
  auto theOsInfo = osInfo();
  std::string base = temporaryFile("wc.test", "unittest");
  ensureDirectory(base.c_str());
  std::string file1 = joinPath(base.c_str(), "file1.txt", nullptr, "dir1");
  std::string file2 = joinPath(base.c_str(), "file2.txt", nullptr, "dir1/dir2");
  ensureDirectory(dirname(file1.c_str()).c_str());
  ensureDirectory(dirname(file2.c_str()).c_str());
  writeText(file1.c_str(),
      R"""(adam: 1234   admin
joe: 1235 user blocked
eve: 1235 user active assistant
)""");
  writeText(file2.c_str(),
      R"""(What should I do?

When my love is away
...
)""");
  return base;
}

int countStartingWith(MemoryAppender *appender, const char *prefix) {
  int rc = 0;
  for (size_t ix = 0; ix < appender->count(); ix++) {
    const std::string &line = appender->at(ix);
    if (startsWith(line.c_str(), line.size(), prefix)) {
      rc++;
    }
  }
  return rc;
}
bool matchInAnyLine(MemoryAppender *appender, const char *toFind) {
  bool rc = false;
  for (size_t ix = 0; ix < appender->count(); ix++) {
    const std::string &line = appender->at(ix);
    if (strstr(line.c_str(), toFind) != nullptr) {
      rc = true;
      break;
    }
  }
  return rc;
}
TEST(FileKnifeTest, list) {
  FEW_TESTS();
  auto base = initTree();
  const char *argv[] = { "-l5", "list", base.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(8, appender->count());
  ASSERT_EQ(4, countStartingWith(appender, " "));
  ASSERT_EQ(2, countStartingWith(appender, "d"));
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000449 MB /tmp/unittest/dir1/file0.dat"));
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000080 MB /tmp/unittest/dir1/file1.txt"));
  ASSERT_TRUE(matchInAnyLine(appender, "directory /tmp/unittest/dir1/dir2"));
  ASSERT_TRUE(
      matchInAnyLine(appender,
          "0.001024 MB /tmp/unittest/dir1/dir2/dir3/file3.dat"));
  delete logger;
}
TEST(FileKnifeTest, listSize) {
  FEW_TESTS();
  auto base = initTree();
  const char *argv[] = { "list", "--size=+100", "-s-300", base.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(3, appender->count());
  ASSERT_TRUE(
      matchInAnyLine(appender,
          "0.000244 MB /tmp/unittest/dir1/dir2/file2.txt"));
  delete logger;
}
TEST(FileKnifeTest, listFiles) {
  FEW_TESTS();
  auto base = initTree();
  const char *argv[] = { "list", "--files=;*.txt", base.c_str() };
  auto logger = buildMemoryLogger(100, LV_SUMMARY);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines2 = appender->linesAsString();
  auto lines = appender->lines();
  ASSERT_EQ(4, appender->count());
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000080 MB /tmp/unittest/dir1/file1.txt"));
  ASSERT_TRUE(
      matchInAnyLine(appender,
          "0.000244 MB /tmp/unittest/dir1/dir2/file2.txt"));
  delete logger;
}

TEST(FileKnifeTest, listDirectories) {
  FEW_TESTS();
  auto base = initTree();
  const char *argv[] = { "-l8", "list", "--directories=;-dir2;-dir3",
      base.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(5, appender->count());
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000449 MB /tmp/unittest/dir1/file0.dat"));
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000080 MB /tmp/unittest/dir1/file1.txt"));
  ASSERT_TRUE(
      matchInAnyLine(appender,
          "2 file(s)     0.000529 MByte       1 dirs(s) Ignored:       2  dir(s)"));
  ASSERT_TRUE(matchInAnyLine(appender, "directory /tmp/unittest/dir1/dir2"));
  delete logger;
  delete logger;
}

TEST(FileKnifeTest, listDirectoriesMinutes) {
  FEW_TESTS();
  auto base = initTree();
  const char *argv[] = { "-l8", "list", "--minutes=-30", base.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(8, appender->count());
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000449 MB /tmp/unittest/dir1/file0.dat"));
  ASSERT_TRUE(
      matchInAnyLine(appender, "0.000080 MB /tmp/unittest/dir1/file1.txt"));
  ASSERT_TRUE(
      matchInAnyLine(appender,
          "4 file(s)     0.001797 MByte       3 dirs(s) Ignored:       0  dir(s)       0  file(s)"));
  ASSERT_TRUE(matchInAnyLine(appender, "directory /tmp/unittest/dir1/dir2"));
}

TEST(FileKnifeTest, extrema) {
  FEW_TESTS();
  auto base = initTree();
  const char *argv[] = { "extrema", "--count=2", base.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(11, appender->count());
  delete logger;
}

TEST(FileKnifeTest, wc) {
  FEW_TESTS();
  auto base = initTreeWc();
  const char *argv[] = { "wc", base.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_TRUE(startsWith(lines[0].c_str(), -1, "4 10 44"));
  ASSERT_TRUE(startsWith(lines[1].c_str(), -1, "3 12 74"));
  ASSERT_TRUE(startsWith(lines[2].c_str(), -1, "7 22 118"));
  appender->clear();
  const char *argv2[] = { "wc", "-l", base.c_str() };
  fileKnife(sizeof argv2 / sizeof argv2[0], const_cast<char**>(argv2), logger);
  lines = appender->lines();
  ASSERT_TRUE(
      startsWith(lines[0].c_str(), -1, (std::string("4 ") + base).c_str()));
  appender->clear();
  const char *argv3[] = { "wc", "-w", base.c_str() };
  fileKnife(sizeof argv3 / sizeof argv3[0], const_cast<char**>(argv3), logger);
  lines = appender->lines();
  ASSERT_TRUE(
      startsWith(lines[0].c_str(), -1, (std::string("10 ") + base).c_str()));
  appender->clear();
  const char *argv4[] = { "wc", "-b", base.c_str() };
  fileKnife(sizeof argv4 / sizeof argv4[0], const_cast<char**>(argv4), logger);
  lines = appender->lines();
  ASSERT_TRUE(
      startsWith(lines[0].c_str(), -1, (std::string("44 ") + base).c_str()));
  appender->clear();
  auto pattern = base + "/*.txt,*.dat";
  const char *argv5[] = { "wc", "-L", pattern.c_str() };
  fileKnife(sizeof argv5 / sizeof argv5[0], const_cast<char**>(argv5), logger);
  lines = appender->lines();
  ASSERT_TRUE(
      startsWith(lines[0].c_str(), -1, (std::string("20 ") + base).c_str()));
  delete logger;
}

