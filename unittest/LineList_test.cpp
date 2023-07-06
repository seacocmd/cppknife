/*
 * LineList_test.cpp
 *
 *  Created on: 10.04.2022
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

TEST(LineListTest, basics) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  auto theOsInfo(osInfo());
  LineList list1(10, logger);
  bool differentLength = false;
  list1.lines() = splitCString("abc\n123\none two three", "\n");
  auto fn = theOsInfo._tempDirectorySeparator + "linelist.test.txt";
  list1.writeToFile(fn.c_str());
  ASSERT_TRUE(fileExists(fn.c_str()));
  LineList list2(10, logger);
  list2.readFromFile(fn.c_str(), true);
  ASSERT_EQ(-1, list2.indexOfFirstDifference(list1));
  list2.lines()[1] += "!";
  ASSERT_EQ(1, list2.indexOfFirstDifference(list1));
  ASSERT_EQ(1, list1.indexOfFirstDifference(list2));
  ASSERT_EQ(-1, list1.indexOfFirstDifference(list2, 2));
  list2 = list1;
  ASSERT_EQ(-1, list2.indexOfFirstDifference(list1));
  list2.lines().push_back("doobeedo");
  ASSERT_EQ(3, list1.indexOfFirstDifference(list2, 0, &differentLength));
  ASSERT_TRUE(differentLength);
  list2 = list1;
  ASSERT_EQ(-1, list2.indexOfFirstDifference(list1));
  LineList list3(list1);
  ASSERT_EQ(-1, list2.indexOfFirstDifference(list1));
  delete logger;
}
TEST(LineListTest, find) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  list1.lines() = splitCString("number = 123\nbool=True\nstring =abc", "\n");
  ASSERT_EQ(0, list1.find("number\\s="));
  ASSERT_EQ(-1, list1.find("number\\s=", 1));
  ASSERT_EQ(2, list1.find("[a-c]{3}", 1));
  delete logger;
}
TEST(LineListTest, adapt) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  list1.lines() = splitCString("number = 123\nbool=True\nstring =abc", "\n");
  ASSERT_EQ(CT_UNCHANGED, list1.adapt("number\\s=", "number = 123"));
  ASSERT_FALSE(list1.hasChanged());
  ASSERT_EQ(CT_CHANGED, list1.adapt("number\\s=", "number = 432"));
  ASSERT_STREQ("number = 432", list1.constLines()[0].c_str());
  ASSERT_TRUE(list1.hasChanged());
  delete logger;
}
TEST(LineListTest, adaptFromConfigurationReplace) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Configuration config(logger);
  config.populate(R"""(max-memory=512k
max.files=100
)""");
  LineList list1(10, logger);
  list1.lines() =
  splitCString(
      R"""(number = 123
bool=True
max-memory = 2k
string =abc
max.files = 2
)""",
      "\n");
  list1.adaptFromConfiguration(config, *logger);
  auto lines =
  dynamic_cast<MemoryAppender*>(logger->findAppender("memory"))->linesAsString();
  ASSERT_STREQ("= changed: max-memory = 512k\n= changed: max.files = 100",
      lines.c_str());
  delete logger;
}
TEST(LineListTest, adaptFromConfigurationInserted) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Configuration config(logger);
  config.populate(
      R"""(max-memory=512k
_anchor_.max-memory = ^#\s*max-memory
max.files=100
_anchor_.max.files = ^\[files\]
)""");
  LineList list1(10, logger);
  list1.lines() =
  splitCString(
      R"""(number = 123
bool=True
#max-memory = 2k
string =abc
[files]
dummy = false
)""",
      "\n");
  list1.adaptFromConfiguration(config, *logger);
  auto lines =
  dynamic_cast<MemoryAppender*>(logger->findAppender("memory"))->linesAsString();
  ASSERT_STREQ("= inserted: max-memory = 512k\n= inserted: max.files = 100",
      lines.c_str());
  delete logger;
}

TEST(LineListTest, adaptFromConfigurationAppended) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Configuration config(logger);
  config.populate(
      R"""(max-memory=512k
_anchor_.max-memory = ^#\s*max-memory
max.files=100
_anchor_.max.files = ^\[files\]
)""");
  LineList list1(10, logger);
  list1.lines() = splitCString(
      R"""(number = 123
bool=True
string =abc
dummy = false
)""", "\n");
  list1.adaptFromConfiguration(config, *logger);
  auto lines =
  dynamic_cast<MemoryAppender*>(logger->findAppender("memory"))->linesAsString();
  ASSERT_STREQ("= appended: max-memory = 512k\n= appended: max.files = 100",
      lines.c_str());
  delete logger;
}

TEST(LineListTest, deleteRangeOneLine) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(5, logger);
  auto data = splitCString(
      R"""(Line 1 abc
Line 2 def
Line 3 xyz
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  BufferPosition start;
  BufferPosition end;
  start._lineIndex = 1;
  start._columnIndex = 4;
  end._lineIndex = 1;
  end._columnIndex = 99;
  list1.deleteRange(start, end);
  start._lineIndex = 2;
  start._columnIndex = 2;
  end._lineIndex = 2;
  end._columnIndex = 5;
  list1.deleteRange(start, end);
  auto data2 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data2.c_str(), R"""(Line 1 abc
Line
Li3 xyz
0123456789
)""");
  delete logger;
}

TEST(LineListTest, deleteRangeMoreLinesFullLines) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(5, logger);
  auto data = splitCString(
      R"""(Line 1 abc
Line 2 def
Line 3 xyz
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  BufferPosition start;
  BufferPosition end;
  start._lineIndex = 1;
  start._columnIndex = 0;
  end._lineIndex = 3;
  end._columnIndex = 0;
  list1.deleteRange(start, end);
  auto data2 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data2.c_str(), R"""(Line 1 abc
0123456789
)""");
  delete logger;
}

TEST(LineListTest, deleteRangeMoreLinesPartLines) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data = splitCString(
      R"""(Line 1 abc
Line 2 def
Line 3 xyz
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  BufferPosition start;
  BufferPosition end;
  start._lineIndex = 0;
  start._columnIndex = 2;
  end._lineIndex = 3;
  end._columnIndex = 4;
  list1.deleteRange(start, end);
  auto data2 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data2.c_str(), R"""(Li456789
)""");
  delete logger;
}

TEST(LineListTest, deleteRangeInvertedStartEnd) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data = splitCString(
      R"""(Line 1 abc
Line 2 def
Line 3 xyz
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  BufferPosition start;
  BufferPosition end;
  end._lineIndex = 0;
  end._columnIndex = 2;
  start._lineIndex = 3;
  start._columnIndex = 4;
  list1.deleteRange(start, end);
  auto data2 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data2.c_str(), R"""(Li456789
)""");
  delete logger;
}

TEST(LineListTest, insertOneLine) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data = splitCString(R"""(Line 1 abc
Line 2 def
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  BufferPosition position;
  position._lineIndex = 1;
  position._columnIndex = 6;
  list1.insert(position, "Wow");
  auto data2 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data2.c_str(), R"""(Line 1 abc
Line 2Wow def
0123456789
)""");
  delete logger;
}

TEST(LineListTest, insertMoreLinesNoNewline) {
  //FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  LineList list2(10, logger);
  auto data = splitCString(R"""(Line 1 abc
Line 2 def
0123456789
)""", "\n");
  auto data2 = splitCString(R"""(xxx
yyyy
zz)""", "\n");
  list1.lines() = std::vector(data);
  auto lines2 = std::vector(data2);
  BufferPosition position(1, 6);
  list1.insert(position, lines2, false);
  auto data3 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data3.c_str(),
      R"""(Line 1 abc
Line 2xxx
yyyy
zz def
0123456789
)""");
  delete logger;
}

TEST(LineListTest, insertMoreLinesAddNewline) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  LineList list2(10, logger);
  auto data = splitCString(R"""(Line 1 abc
Line 2 def
0123456789
)""", "\n");
  auto data2 = splitCString(R"""(xxx
yyyy
zz)""", "\n");
  list1.lines() = std::vector(data);
  auto lines2 = std::vector(data2);
  BufferPosition position(1, 6);
  list1.insert(position, lines2);
  auto data3 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data3.c_str(),
      R"""(Line 1 abc
Line 2xxx
yyyy
zz
 def
0123456789
)""");
  delete logger;
}

TEST(LineListTest, insertMoreLinesAddNewlineBeginOfLine) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  LineList list2(10, logger);
  auto data = splitCString(R"""(Line 1 abc
Line 2 def
)""", "\n");
  auto data2 = splitCString(R"""(xxx
yyyy
zz)""", "\n");
  list1.lines() = std::vector(data);
  auto lines2 = std::vector(data2);
  BufferPosition position(1, 0);
  list1.insert(position, lines2);
  auto data3 = joinVector(list1.constLines(), "\n");
  ASSERT_STREQ(data3.c_str(),
      R"""(Line 1 abc
xxx
yyyy
zz
Line 2 def
)""");
  delete logger;
}

TEST(LineListTest, searchInline) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data = splitCString(R"""(Line 1 abc
Line 2 def
)""", "\n");
  list1.lines() = std::vector(data);
  BufferPosition position;
  SearchResult result;
  SearchExpression searchExpression("def", false, "L");
  ASSERT_FALSE(list1.search(searchExpression, result, true));
  ASSERT_EQ(list1.position(position)._lineIndex, 0);
  ASSERT_EQ(position._columnIndex, 0);
  delete logger;
}

TEST(LineListTest, searchBackwardsInline) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data = splitCString(R"""(Line 1 abc
a1b a2ba3b
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  SearchResult result;
  BufferPosition position;
  list1.setPosition(1, 99);
  SearchExpression searchExpression("a.b", true, "BL");
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_STREQ("a3b", list1.lastHit().c_str());
  ASSERT_EQ(9, list1.position(position)._columnIndex);
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_STREQ("a2b", list1.lastHit().c_str());
  ASSERT_EQ(6, list1.position(position)._columnIndex);
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_STREQ("a1b", list1.lastHit().c_str());
  ASSERT_EQ(2, list1.position(position)._columnIndex);
  ASSERT_FALSE(list1.search(searchExpression, result, true));
  delete logger;
}

TEST(LineListTest, searchBackwards) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data = splitCString(
      R"""(Line 1 a1b
Line2 a2b
Line  3 a3b
0123456789
)""", "\n");
  list1.lines() = std::vector(data);
  SearchResult result;
  BufferPosition position;
  list1.setPosition(99, 0);
  SearchExpression searchExpression("a.b", true, "B");
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_STREQ("a3b", list1.lastHit().c_str());
  ASSERT_EQ(10, list1.position(position)._columnIndex);
  ASSERT_EQ(2, position._lineIndex);
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_STREQ("a2b", list1.lastHit().c_str());
  ASSERT_EQ(8, list1.position(position)._columnIndex);
  ASSERT_EQ(1, position._lineIndex);
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_STREQ("a1b", list1.lastHit().c_str());
  ASSERT_EQ(9, list1.position(position)._columnIndex);
  ASSERT_EQ(0, position._lineIndex);
  ASSERT_FALSE(list1.search(searchExpression, result, true));
  delete logger;
}

TEST(LineListTest, replaceString) {
  FEW_TESTS;
  std::string data("a1b a2b a3ba4b");
  SearchExpression searchExpression("a.b", true, "");
  auto string = data;
  ASSERT_EQ(1, LineList::replaceString(string, searchExpression, "x", 1));
  ASSERT_STREQ("x a2b a3ba4b", string.c_str());
  string = data;
  ASSERT_EQ(4, LineList::replaceString(string, searchExpression, "x"));
  ASSERT_STREQ("x x xx", string.c_str());
}

TEST(LineListTest, replaceOneLine) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data =
  splitCString(
      R"""(Line 1 a1b
Line2 a2b abb a4b
Line  3 a3b axb
Line4ayb
0123456789 123456
)""",
      "\n");
  list1.lines() = std::vector(data);
  list1.setPosition(2, 0);
  SearchExpression searchExpression("a.b", true, "L");
  ASSERT_EQ(2, list1.replace(searchExpression, "XXXX"));
  ASSERT_STREQ(list1.constLines()[1].c_str(), "Line2 a2b abb a4b");
  ASSERT_STREQ(list1.constLines()[2].c_str(), "Line  3 XXXX XXXX");
  ASSERT_STREQ(list1.constLines()[3].c_str(), "Line4ayb");
  BufferPosition start(1, 7);
  BufferPosition end(1, 15);
  searchExpression.set("a.b", true);
  ASSERT_EQ(1, list1.replace(searchExpression, "Y", -1, &start, &end));
  ASSERT_STREQ(list1.constLines()[0].c_str(), "Line 1 a1b");
  ASSERT_STREQ(list1.constLines()[1].c_str(), "Line2 a2b Y a4b");
  ASSERT_STREQ(list1.constLines()[2].c_str(), "Line  3 XXXX XXXX");
  delete logger;
}

TEST(LineListTest, replaceTwoLines) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data =
  splitCString(
      R"""(
Line 1 a1b aXb
Line2 a2b abb a4b
Line  3 a3b axb
Line4ayb
0123456789 123456
)""",
      "\n");
  list1.lines() = std::vector(data);
  list1.setPosition(2, 0);
  SearchExpression searchExpression("a.b", true);
  BufferPosition start(1, 8);
  BufferPosition end(2, 12);
  ASSERT_EQ(2, list1.replace(searchExpression, "Z", -1, &start, &end));
  ASSERT_STREQ(list1.constLines()[1].c_str(), "Line 1 a1b Z");
  ASSERT_STREQ(list1.constLines()[2].c_str(), "Line2 Z abb a4b");
  ASSERT_STREQ(list1.constLines()[3].c_str(), "Line  3 a3b axb");
  ASSERT_EQ(6, list1.replace(searchExpression, "%"));
  ASSERT_STREQ(list1.constLines()[1].c_str(), "Line 1 % Z");
  ASSERT_STREQ(list1.constLines()[2].c_str(), "Line2 Z % %");
  ASSERT_STREQ(list1.constLines()[3].c_str(), "Line  3 % %");
  ASSERT_STREQ(list1.constLines()[4].c_str(), "Line4%");
  delete logger;
}

TEST(LineListTest, replaceFilterMatches) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data =
  splitCString(
      R"""(
Line 1 a1b aXb
Line2 a2b abb a4b
Line  3 a3b axb
Line4ayb
0123456789 123456
)""",
      "\n");
  list1.lines() = std::vector(data);
  list1.setPosition(2, 0);
  SearchExpression searchExpression("a.b", true);
  SearchExpression filter("line", true, "i");
  BufferPosition start(1, 8);
  BufferPosition end(2, 12);
  ASSERT_EQ(2, list1.replace(searchExpression, "Z", -1, &start, &end, &filter));
  ASSERT_STREQ(list1.constLines()[1].c_str(), "Line 1 a1b Z");
  ASSERT_STREQ(list1.constLines()[2].c_str(), "Line2 Z abb a4b");
  ASSERT_STREQ(list1.constLines()[3].c_str(), "Line  3 a3b axb");
  ASSERT_EQ(6, list1.replace(searchExpression, "%"));
  ASSERT_STREQ(list1.constLines()[1].c_str(), "Line 1 % Z");
  ASSERT_STREQ(list1.constLines()[2].c_str(), "Line2 Z % %");
  ASSERT_STREQ(list1.constLines()[3].c_str(), "Line  3 % %");
  ASSERT_STREQ(list1.constLines()[4].c_str(), "Line4%");
  delete logger;
}

TEST(LineListTest, replaceFilterBlocks) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data =
  splitCString(
      R"""(
Line 1 a1b aXb
Line2 a2b abb a4b
Line  3 a3b axb
Line4ayb
0123456789 123456
)""",
      "\n");
  list1.lines() = std::vector(data);
  list1.setPosition(2, 0);
  SearchExpression searchExpression("a.b", true);
  SearchExpression filter("<nothing>", true, "i");
  BufferPosition start(1, 8);
  BufferPosition end(2, 12);
  ASSERT_EQ(0, list1.replace(searchExpression, "Z", -1, &start, &end, &filter));
  ASSERT_EQ(list1.constLines(), data);
  delete logger;
}

TEST(LineListTest, replaceStringBackreferences) {
  FEW_TESTS;
  SearchExpression searchExpression("(\\S+),(\\S+)", true);
  std::regex reBackReferences("!(\\d+)");
  std::string line(" 123,456 ");
  ASSERT_EQ(1,
      LineList::replaceString(line, searchExpression, "X!2,!1Z", -1,
          &reBackReferences));
  ASSERT_STREQ(line.c_str(), " X456,123Z ");
}

TEST(LineListTest, copyRange) {
  FEW_TESTS;
  std::vector<std::string> target;
  BufferPosition start;
  BufferPosition end(LineList::END_OF_FILE, 0);
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  auto data =
  splitCString(
      R"""(Line1
Line2 abc
Line3 ABCDE
0123456789 1
)""",
      "\n");
  list1.setLines(data);
  // Whole buffer:
  list1.copyRange(target, start, end);
  ASSERT_EQ(data, target);
  start.set(2, 2);
  end.set(2, 5);
  target.clear();
  // range in one line:
  list1.copyRange(target, start, end);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "ne3");
  start.set(2, 2);
  end.set(2, 4);
  target.clear();
  // range in one line, including end:
  list1.copyRange(target, start, end, false);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "ne3");
  start.set(2, 3);
  end.set(3, 5);
  target.clear();
  // 2 lines: ranges in start and in end:
  list1.copyRange(target, start, end, false);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "e3 ABCDEX012345");
  start.set(2, 4);
  end.set(3, 6);
  target.clear();
  // start range, end range
  list1.copyRange(target, start, end);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "3 ABCDEX012345");
  start.set(1, 4);
  end.set(3, 7);
  target.clear();
  // start range, full line end range
  list1.copyRange(target, start, end);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "2 abcXLine3 ABCDEX0123456");
  start.set(1, 0);
  end.set(3, 0);
  target.clear();
  // 3 full lines
  list1.copyRange(target, start, end);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "Line2 abcXLine3 ABCDE");
  start.set(1, 9);
  end.set(2, 0);
  target.clear();
  // Invalid start range:
  list1.copyRange(target, start, end);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "");
  start.set(1, 8);
  end.set(2, 99);
  target.clear();
  // Invalid end range:
  list1.copyRange(target, start, end);
  ASSERT_STREQ (joinVector(target, "X").c_str(), "cXLine3 ABCDE");
  delete logger;
}

TEST(LineListTest, searchFlagsPosition) {
  FEW_TESTS;
  std::vector<std::string> target;
  auto logger = buildMemoryLogger();
  LineList list1(10, logger);
  BufferPosition position;
  auto data =
  splitCString(
      R"""(Line1
axc abc aXc
Line3 auc
0123456789 1
)""",
      "\n");
  list1.setLines(data);
  SearchExpression searchExpression("abc", true, "<");
  SearchResult result;
  list1.setPosition(3, 99);
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_EQ(result._position._lineIndex, 1);
  ASSERT_EQ(result._position._columnIndex, 4);
  ASSERT_EQ(list1.position(position)._lineIndex, 1);
  ASSERT_EQ(list1.position(position)._columnIndex, 7);
  searchExpression.set("a.c", true, "^");
  ASSERT_TRUE(list1.search(searchExpression, result, false));
  ASSERT_EQ(result._position._lineIndex, 1);
  ASSERT_EQ(result._position._columnIndex, 0);
  ASSERT_EQ(list1.position(position)._lineIndex, 1);
  ASSERT_EQ(list1.position(position)._columnIndex, 7);
  searchExpression.set("a.c", true, "$");
  ASSERT_TRUE(list1.search(searchExpression, result, true));
  ASSERT_EQ(result._position._lineIndex, 2);
  ASSERT_EQ(result._position._columnIndex, 6);
  delete logger;
}
