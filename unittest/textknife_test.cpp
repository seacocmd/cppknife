/*
 * textknife_test.cpp
 *
 *  Created on: 07.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../tools/textknife.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

std::string initTree() {
  auto base =
      buildFileTree(
          R"""(/tmp/unittest/text/file1.txt;80
/tmp/unittest/text/dir2/file2.txt;244
/tmp/unittest/text/dir2/dir3/file3.dat;1024
)""");
  base = base.substr(0, base.size() - 1);
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
TEST(TextKnifeTest, adaptPhp) {
  FEW_TESTS();
  auto theOsInfo = osInfo();
  auto filename = temporaryFile("php.conf", "unittest/adapt", true);
  writeText(filename.c_str(),
      R"""(memory_limit = 33M
additional = 44
upload_max_filesize = 330k
)""");
  const char *argv[] = { "-l5", "adapt", "--template=php", filename.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  textKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(13, appender->count());
  auto current = readAsString(filename.c_str());
  ASSERT_STREQ(current.c_str(),
      R"""(memory_limit = 2048M
additional = 44
upload_max_filesize = 512M
default_socket_timeout = 600
max_execution_time = 600
max_file_uploads = 100
max_input_time = 600
opcache.enable = 1
opcache.memory_consumption = 1024
opcache.interned_strings_buffer = 512
post_max_size = 512M
session.save_handler = redis
session.save_path = "tcp://127.0.0.1:6379"
)""");
}
TEST(TextKnifeTest, adaptSingleExists) {
  FEW_TESTS();
  return;
  auto theOsInfo = osInfo();
  auto filename = temporaryFile("single.conf", "unittest/adapt", true);
  writeText(filename.c_str(), R"""(line 1
Max-memory = 3
line 2
)""");
  const char *argv[] = { "-l5", "adapt", "--pattern=/^\\s*max-memory\\s*=/i",
      "--replacement=max-memory = 9k", filename.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  textKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(13, appender->count());
  auto current = readAsString(filename.c_str());
  ASSERT_STREQ(current.c_str(), R"""(line 1
max-memory = 9k
line 2
)""");
  delete logger;
}

TEST(TextKnifeTest, strings) {
  FEW_TESTS();
  auto theOsInfo = osInfo();
  auto input = temporaryFile("strings.txt", "unittest", true);
  auto output = temporaryFile("strings.srt.txt", "unittest", true);
  const char *text =
      R"""(
      1: 'c'
      'b' and 'a'
      "d" und "e" und "'tick'"
      "12\"attr\"34"
      )""";
  writeText(input.c_str(), text);
  const char *argv[] = { "-l5", "strings", "-o", output.c_str(), input.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  textKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(1, appender->count());
  auto current = readAsString(output.c_str());
  ASSERT_STREQ(current.c_str(),
      R"""("'tick'"
"12\"attr\"34"
"d"
"e"
'a'
'b'
'c'
)""");
  delete logger;
}
TEST(TextKnifeTest, search) {
  FEW_TESTS();
  auto theOsInfo = osInfo();
  auto input = temporaryFile("search1.txt", "unittest", true);
  auto input2 = temporaryFile("search2.txt", "unittest", true);
  const char *text =
      R"""(With a little help from my friends
No woman no cry
Mandy)""";
  writeText(input.c_str(), text);
  const char *text2 = R"""(Norwegian wood
Help
Suzy Q)""";
  writeText(input2.c_str(), text2);
  auto source = input;
  replaceString(source, "1", "*");
  const char *argv[] = { "search", "-P/.*y.*/i", source.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  textKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(5, appender->count());
  delete logger;
}
TEST(TextKnifeTest, searchOnlyMatch) {
  //FEW_TESTS();
  auto theOsInfo = osInfo();
  auto input = temporaryFile("search1.txt", "unittest", true);
  auto input2 = temporaryFile("search2.txt", "unittest", true);
  const char *text =
      R"""(With a little help from my friends
No woman no cry
Mandy)""";
  writeText(input.c_str(), text);
  const char *text2 = R"""(Norwegian wood
Help
Suzy Q)""";
  writeText(input2.c_str(), text2);
  auto source = input;
  replaceString(source, "1", "*");
  const char *argv[] = { "search", "-o", "-P/y/i", "--max-count=1",
      source.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  textKnife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  ASSERT_EQ(3, appender->count());
  delete logger;
}

