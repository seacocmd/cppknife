/*
 * FileTool_test.cpp
 *
 *  Created on: 24.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../os/os.hpp"
#include "google_test.hpp"
#include "../tools/fileknife.hpp"

using namespace cppknife;

static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return
#if ! defined(TEST)
#define TEST(a,b) void b::a()
#endif

TEST(FileToolTest, ensureDirectory) {
  FEW_TESTS;
  if (osInfo()._osType == LINUX) {
    ensureDirectory("/tmp/unittest/ensure.dir");
    ASSERT_TRUE(ensureDirectory("/tmp/unittest/ensure.dir"));
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileToolTest, fileExists) {
  FEW_TESTS;
  if (osInfo()._osType == LINUX) {
    ASSERT_TRUE(fileExists("/etc/passwd"));
    ASSERT_FALSE(fileExists("/jdkfdkla/jfdkafdka"));
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileToolTest, isDirectory) {
  bool exists;
  if (osInfo()._osType == LINUX) {
    ASSERT_TRUE(isDirectory("/etc/default", &exists));
    ASSERT_TRUE(exists);
    exists = false;
    ASSERT_FALSE(isDirectory("/etc/passwd", &exists));
    ASSERT_TRUE(exists);
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileToolTest, makeDirectory) {
  FEW_TESTS;
  if (osInfo()._osType == LINUX) {
    ASSERT_TRUE(makeDirectory("/tmp/unittest/dir1/dir2", true) >= 0);
    ASSERT_EQ(makeDirectory("/tmp/unittest"), 0);
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileToolTest, buildFileTree) {
  FEW_TESTS;
  if (osInfo()._osType == LINUX) {
    auto base =
    buildFileTree(
        R"""(/tmp/unittest/dir1/file0.dat;449
/tmp/unittest/dir1/file1.txt;80
/tmp/unittest/dir1/dir2/file2.txt;244
)""");
    ASSERT_TRUE(fileExists("/tmp/unittest/dir1/file0.dat"));
    ASSERT_TRUE(fileExists("/tmp/unittest/dir1/file1.txt"));
    ASSERT_TRUE(fileExists("/tmp/unittest/dir1/dir2/file2.txt"));
  } else {
    throw InternalError("missing FileTool::buildFileTree test");
  }
}
TEST(FileToolTest, fileTimeCompare) {
  FEW_TESTS;
  if (osInfo()._osType == LINUX) {
    FileTime_t t1;
    FileTime_t t2;
    FileTime_t t3;
    setFiletime(t1, 1234999, 32);
    setFiletime(t2, 1234999, 34);
    setFiletime(t3, 1235000, 8);
    ASSERT_TRUE(fileTimeCompare(t1, t2) < 0);
    ASSERT_TRUE(fileTimeCompare(t3, t2) > 0);
    ASSERT_TRUE(fileTimeCompare(t1, t3) < 0);
    ASSERT_TRUE(fileTimeCompare(t1, t1) == 0);
  } else {
    throw InternalError("missing FileTool::buildFileTree test");
  }
}
TEST(FileToolTest, filetimeToString) {
  FEW_TESTS;
  FileTime_t t1;
  t1.tv_sec = 258428940;
  t1.tv_nsec = 123100100;
  auto t2 = filetimeToString(t1);
  ASSERT_STREQ(t2.c_str(), "1978.03.11 02:49:00");
}

TEST(FileToolTest, textToFile) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn = theOsInfo._tempDirectorySeparator + "texttofile.test.txt";
  ASSERT_TRUE(writeText(fn.c_str(), "1234\n6789\n"));
  File file(fn.c_str());
  ASSERT_EQ(10, file.size());
}
TEST(FileToolTest, writeBinary) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn = theOsInfo._tempDirectorySeparator + "writebinary.test.data";
  char data[] = {0, 1, 2, 3, 4, 5, 0};
  ASSERT_TRUE(writeBinary(fn.c_str(), data, sizeof data));
  File file(fn.c_str());
  ASSERT_EQ(7, file.size());
}
TEST(FileToolTest, isBinary) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn2 = theOsInfo._tempDirectorySeparator + "isbinary.test.data";
  auto fn = fn2.c_str();
  char data1[] = {1, 2, 'a', 'b', 3, 4, 'c', 5, 'x', 'y'};
  ASSERT_TRUE(writeBinary(fn, data1, sizeof data1));
  ASSERT_TRUE(isBinary(fn));
  char data2[] = {1, 2, 3, 4, 'a', 'b', 'c', 'x', 'y'};
  ASSERT_TRUE(writeBinary(fn, data2, sizeof data2));
  ASSERT_FALSE(isBinary(fn));
  ASSERT_TRUE(
      writeText(fn, "A yellow submarine\n\tx\n\tWow\r\n\v... and more\n"));
  ASSERT_FALSE(isBinary(fn));
}
TEST(FileToolTest, listFiles) {
  FEW_TESTS;
  auto files = listFiles("/etc", "^(passwd|group)$");
  ASSERT_EQ(2, files.size());
  std::sort(files.begin(), files.end());
  ASSERT_STREQ("group", files[0].c_str());
  ASSERT_STREQ("passwd", files[1].c_str());
  files = listFiles("/etc", "^(Passwd|Group)$", true, true);
  ASSERT_EQ(2, files.size());
  std::sort(files.begin(), files.end());
  ASSERT_STREQ("/etc/group", files[0].c_str());
  ASSERT_STREQ("/etc/passwd", files[1].c_str());
}
TEST(FileToolTest, readAsString) {
  FEW_TESTS;
  const char* data = R"""(line one
line two
)""";
  auto theOsInfo(osInfo());
  auto logger = buildMemoryLogger(10, LV_FINEST);
  auto fn2 = theOsInfo._tempDirectorySeparator + "readAsString.test.data";
  auto fn = fn2.c_str();
  ASSERT_TRUE(writeText(fn, data, -1));
  auto data2 = readAsString(fn);
  ASSERT_STREQ(data, data2.c_str());
  fn2 += "!";
  data2 = readAsString(fn, logger, 2);
  ASSERT_STREQ("", data2.c_str());
  data2 = readAsString(fn);
  ASSERT_STREQ("", data2.c_str());
  delete logger;
}
TEST(FileToolTest, readAsList) {
  FEW_TESTS;
  const char* data = R"""(line one
line two
line 3)""";
  auto theOsInfo(osInfo());
  auto logger = buildMemoryLogger(10, LV_FINEST);
  auto fn2 = theOsInfo._tempDirectorySeparator + "readAsString.test.data";
  auto fn = fn2.c_str();
  ASSERT_TRUE(writeText(fn, data, -1));
  auto list = readAsList(fn, logger);
  ASSERT_EQ(3, list.size());
  ASSERT_STREQ("line one", list[0].c_str());
  ASSERT_STREQ("line two", list[1].c_str());
  ASSERT_STREQ("line 3", list[2].c_str());
}

TEST(FileToolTest, owner) {
  FEW_TESTS;
  const char* data = R"""(line one
line two
line 3)""";
  auto theOsInfo(osInfo());
  auto logger = buildMemoryLogger(10, LV_FINEST);
  auto target = theOsInfo._tempDirectorySeparator + "etc_owner.data";
  const char *argv[] = {"owner", "/etc", target.c_str()};
  //fileKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
}
