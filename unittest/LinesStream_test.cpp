/*
 * LineStream_test.cpp
 *
 *  Created on: 11.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../text/text.hpp"
#include "google_test.hpp"

using namespace cppknife;

static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

TEST(LinesStreamTest, fileLinesStream) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linesstream.txt", "unittest", true);
  writeText(fnSource.c_str(), R"(1
abc
ABCDEFGH
)");
  FileLinesStream stream(fnSource.c_str(), *logger);
  ASSERT_FALSE(stream.endOfFile());
  std::string text;
  ASSERT_TRUE(stream.fetch(text));
  ASSERT_STREQ("1", text.c_str());
  ASSERT_TRUE(stream.fetch(text));
  ASSERT_STREQ("abc", text.c_str());
  ASSERT_TRUE(stream.fetch(text));
  ASSERT_STREQ("ABCDEFGH", text.c_str());
  ASSERT_FALSE(stream.fetch(text));
  ASSERT_FALSE(stream.fetch(text));

  ASSERT_TRUE(stream.endOfFile());
  delete logger;
}

TEST(LinesStreamTest, stringLinesStream) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  StringLinesStream stream("Abcd");
  std::string text;
  ASSERT_TRUE(stream.fetch(text));
  ASSERT_STREQ("Abcd", text.c_str());
  ASSERT_FALSE(stream.fetch(text));
  ASSERT_FALSE(stream.fetch(text));

  delete logger;
}

