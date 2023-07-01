/*
 * Matcher_test.cpp
 *
 *  Created on: 11.03.2022
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

TEST(MatcherTest, simpleMatcher) {
  FEW_TESTS;
  SimpleMatcher matcher("*.c");
  ASSERT_TRUE(matcher.match("blabla.c"));
  ASSERT_FALSE(matcher.match("hubble.h"));
  ASSERT_FALSE(matcher.match("hubble.cpp"));
  matcher.compile("*test*me*.*");
  ASSERT_TRUE(matcher.match("testme.c"));
  ASSERT_TRUE(matcher.match("Run tests and meet all."));
  ASSERT_FALSE(matcher.match("test and more."));
}
TEST(MatcherTest, patternListMatchIgnoreCase) {
  FEW_TESTS;
  PatternList patterns;
  patterns.set(",*.c,*.h", -1, true);
  ASSERT_FALSE(patterns.match("a.hpp"));
  ASSERT_TRUE(patterns.match("/x/a.c"));
  ASSERT_TRUE(patterns.match("a.CPP", 3));
  ASSERT_TRUE(patterns.match("A.h"));
  ASSERT_FALSE(patterns.match("a.hpp"));
}
TEST(MatcherTest, patternListMatch) {
  FEW_TESTS;
  PatternList patterns;
  patterns.set(";x*.c;y*.h", -1, false);
  ASSERT_TRUE(patterns.match("x/x_a.c"));
  ASSERT_TRUE(patterns.match("y/y_a.h"));
  ASSERT_FALSE(patterns.match("/x/y_a.H"));
}
TEST(MatcherTest, patternListMatchIgnoreCaseNot) {
  FEW_TESTS;
  PatternList patterns;
  patterns.set(",-*.c,-*t*mp*", -1, true);
  ASSERT_TRUE(patterns.match("a.hpp"));
  ASSERT_FALSE(patterns.match("/x/a.c"));
  ASSERT_FALSE(patterns.match("a.CPP", 3));
  ASSERT_TRUE(patterns.match("a.CPP", 4));
  ASSERT_FALSE(patterns.match("temp.CPP", 4));
}

TEST(MatcherTest, patternListSearch) {
  FEW_TESTS;
  PatternList patterns;
  //patterns.set(const char *patterns, bool ignoreCase, const char *separator, const char *notPrefix)
  patterns.set(",x*.c,y*.h", -1, true);
  ASSERT_TRUE(patterns.search("/X/a.c"));
  ASSERT_FALSE(patterns.search("/x/a.c", 5));
  ASSERT_TRUE(patterns.search("y/b/A.h"));
  ASSERT_TRUE(patterns.search("/x/Y/a.hpp"));
  patterns.set(",x*.c,y*.h", -1, false);
  ASSERT_TRUE(patterns.search("/x/a.c"));
  ASSERT_FALSE(patterns.search("/X/a.c"));
  ASSERT_FALSE(patterns.search("/x/a.c", 5));
  ASSERT_TRUE(patterns.search("y/b/a.h"));
  ASSERT_FALSE(patterns.search("y/b/A.H"));
  ASSERT_TRUE(patterns.search("/x/y/a.hpp"));
  ASSERT_FALSE(patterns.search("/x/Y/a.hpp"));
}
TEST(MatcherTest, patternListSearchNot) {
  FEW_TESTS;
  PatternList patterns;
  patterns.set("~x*.c,~t*mp", -1, true, ",", "~");
  ASSERT_TRUE(patterns.search("/X/a.cpp", 5));
  ASSERT_FALSE(patterns.search("/X/a.cpp"));
  ASSERT_TRUE(patterns.search("/X/a.cpp", 5));
  ASSERT_FALSE(patterns.search("a/temp/b"));
}
TEST(MatcherTest, matches) {
  FEW_TESTS;
  ASSERT_TRUE(SimpleMatcher::matches("test*.csv", "testdrive.csv"));
  ASSERT_FALSE(SimpleMatcher::matches("test*.csv", "drive.csv"));
}
TEST(MatcherTest, firstMatch) {
  FEW_TESTS;
  std::vector<std::string> list;
  list.push_back("Adam");
  list.push_back("Bea");
  list.push_back("John");
  ASSERT_EQ(0, SimpleMatcher::firstMatch("A*", list));
  ASSERT_EQ(-1, SimpleMatcher::firstMatch("A*", list, 1));
  ASSERT_EQ(1, SimpleMatcher::firstMatch("*e*", list, 0));
}
TEST(MatcherTest, filterMatches) {
  FEW_TESTS;
  std::vector<std::string> list;
  list.push_back("Adam");
  list.push_back("Bea");
  list.push_back("John");
  auto list2 = SimpleMatcher::filterMatches("*a*", list);
  ASSERT_STREQ("Adam,Bea", joinVector(list2, ",").c_str());
}
