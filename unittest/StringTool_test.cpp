/*
 * StringTool_test.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
using namespace cppknife;

static bool fewTests() {
  return true;
}
#define FEW_TESTS if (fewTests()) return

TEST(StringToolTest, appendString) {
  FEW_TESTS;
  std::string string;
  appendString(string, "Hi", " ");
  appendString(string, "World");
  ASSERT_STREQ(string.c_str(), "Hi World");
}
TEST(StringToolTest, copyCString) {
  FEW_TESTS;
  auto s = copyCString("Hi world");
  ASSERT_STREQ(s, "Hi world");
  delete[] s;
  auto s2 = copyCString("Hi world", 2);
  ASSERT_STREQ(s2, "Hi");
  delete[] s2;
}
TEST(StringToolTest, toUpper) {
  FEW_TESTS;
  auto s = toUpper(std::string("Hello"));
  ASSERT_STREQ(s.c_str(), "HELLO");
}
TEST(StringToolTest, toLower) {
  FEW_TESTS;
  auto s = toLower(std::string("Hello"));
  ASSERT_STREQ(s.c_str(), "hello");
}
TEST(StringToolTest, joinCStrings) {
  FEW_TESTS;
  char buffer[100];
  auto ptr = joinCStrings(buffer, sizeof buffer, "Hi", " ", "world", "!");
  ASSERT_STREQ("Hi world!", ptr);
  ASSERT_STREQ("Hi world!", buffer);
  ptr = joinCStrings(buffer, sizeof buffer, nullptr, nullptr, nullptr, nullptr);
  ASSERT_STREQ("", ptr);
}
TEST(StringToolTest, formatCString) {
  FEW_TESTS;
  auto buffer = formatCString("%d %x%c%s", 123, 0xab, ' ', "Wow");
  ASSERT_EQ(std::string("123 ab Wow"), buffer);
}
TEST(StringToolTest, sumOfLength) {
  FEW_TESTS;
  ASSERT_EQ(sumOfLength("1", "12", "123", "1234"), 10);
  ASSERT_EQ(sumOfLength(nullptr, "12", "123", "1234"), 9);
  ASSERT_EQ(sumOfLength("1", nullptr, "123", "1234"), 8);
  ASSERT_EQ(sumOfLength("1", "12", nullptr, "1234"), 7);
  ASSERT_EQ(sumOfLength("1", "12", "123", nullptr), 6);
  ASSERT_EQ(sumOfLength(nullptr, nullptr), 0);
}
TEST(StringToolTest, splitCString) {
  FEW_TESTS;
  auto v2 = splitCString("HelloXXworldXXandXXtheXXrest!", "XX", 3);
  ASSERT_EQ(3, v2.size());
  ASSERT_EQ("Hello", v2[0]);
  ASSERT_EQ("world", v2[1]);
  ASSERT_EQ("andXXtheXXrest!", v2[2]);
  auto v1 = splitCString("Hello world and the rest!", " ");
  ASSERT_EQ(5, v1.size());
  ASSERT_EQ("Hello", v1[0]);
  ASSERT_EQ("world", v1[1]);
  ASSERT_EQ("and", v1[2]);
  ASSERT_EQ("the", v1[3]);
  ASSERT_EQ("rest!", v1[4]);
}
TEST(StringToolTest, truncateCString) {
  FEW_TESTS;
  ASSERT_STREQ("abc", truncateCString("abcde", 3).c_str());
  ASSERT_STREQ("abcde", truncateCString("abcde", 9).c_str());
}
TEST(StringToolTest, joinVector) {
  FEW_TESTS;
  const char *names = "Adam Berta Charly";
  auto v = splitCString(names, " ");
  ASSERT_STREQ(joinVector(v, " ").c_str(), names);
  names = "Hello!";
  v = splitCString(names, "\n");
  ASSERT_STREQ(joinVector(v, "\n").c_str(), names);
  v = splitCString("a b c", " ");
  ASSERT_STREQ(joinVector(v).c_str(), "abc");

}
TEST(StringToolTest, sumOfVectorLengths) {
  FEW_TESTS;
  const char *names = "Adam Berta Charly";
  auto v = splitCString(names, " ");
  ASSERT_EQ(sumOfVectorLengths(v), 15);
  v.clear();
  ASSERT_EQ(sumOfVectorLengths(v), 0);
  v.push_back("");
  ASSERT_EQ(sumOfVectorLengths(v), 0);
}
TEST(StringToolTest, startsWith) {
  FEW_TESTS;
  ASSERT_TRUE(startsWith("abc", -1, "abc"));
  ASSERT_TRUE(startsWith("abc", -1, "ab"));
  ASSERT_TRUE(startsWith("abc", -1, "a"));
  ASSERT_TRUE(startsWith("abc", -1, "abx", 2));
  ASSERT_TRUE(startsWith("abc", 3, "abx", 2));
  ASSERT_FALSE(startsWith("abc", -1, "Abx"));
  ASSERT_FALSE(startsWith("abc", -1, "abx"));
  ASSERT_FALSE(startsWith("abc", 2, "abx", 3));
  ASSERT_FALSE(startsWith("abc", -1, "abx", 3));
  ASSERT_TRUE(startsWith("aBc", -1, "abc", -1, true));
  ASSERT_TRUE(startsWith("abc", -1, "Ab", -1, true));
  ASSERT_TRUE(startsWith("Abc", -1, "a", -1, true));
  ASSERT_TRUE(startsWith("aBc", -1, "abx", 2, true));
}
TEST(StringToolTest, endsWith) {
  FEW_TESTS;
  ASSERT_TRUE(endsWith("abc", -1, "abc"));
  ASSERT_TRUE(endsWith("abc", 3, "abc"));
  ASSERT_TRUE(endsWith("abc", -1, "bc"));
  ASSERT_TRUE(endsWith("abc", -1, "c"));
  ASSERT_TRUE(endsWith("abc", -1, "bcx", 2));
  ASSERT_FALSE(endsWith("abc", 2, "abc"));
  ASSERT_FALSE(endsWith("abc", -1, "ab"));
  ASSERT_FALSE(endsWith("abc", -1, "bcd"));
  ASSERT_FALSE(endsWith("abc", -1, "bcx", 3));
  ASSERT_TRUE(endsWith("aBc", -1, "abc", -1, true));
  ASSERT_TRUE(endsWith("abc", -1, "Bc", -1, true));
  ASSERT_TRUE(endsWith("Abc", -1, "C", -1, true));
  ASSERT_TRUE(endsWith("abc", -1, "Bcx", 2, true));
}
TEST(StringToolTest, strnicmp) {
  FEW_TESTS;
  // equal strings
  ASSERT_EQ(0, strnicmp("abc", "ABC", 3));
  ASSERT_EQ(0, strnicmp("ABC", "abc", 3));
  // different strings, equal length
  ASSERT_LT(strnicmp("abc", "ABx", 3), 0);
  ASSERT_GT(strnicmp("abx", "ABc", 3), 0);
  // different length > n:
  ASSERT_GT(strnicmp("ab", "ABc", 3), 0);
  ASSERT_LT(strnicmp("ABc", "ab", 3), 0);
  ASSERT_EQ(0, strnicmp("", "", 0));
  // n == 0
  ASSERT_EQ(0, strnicmp("A", "a", 0));
  ASSERT_EQ(0, strnicmp("a", "A", 0));
  // n > strlen()
  ASSERT_EQ(0, strnicmp("A", "a", 2));
  ASSERT_EQ(0, strnicmp("a", "A", 2));
}
TEST(StringToolTest, stricmp) {
  FEW_TESTS;
  // equal strings
  ASSERT_EQ(0, stricmp("abc", "ABC"));
  ASSERT_EQ(0, stricmp("ABC", "abc"));
  // different strings, equal length
  ASSERT_LT(stricmp("abc", "ABx"), 0);
  ASSERT_GT(stricmp("abx", "ABc"), 0);
  // different length > n:
  ASSERT_GT(stricmp("ab", "ABc"), 0);
  ASSERT_LT(stricmp("ABc", "ab"), 0);
  ASSERT_EQ(0, stricmp("", ""));
}
TEST(StringToolTest, appendInt) {
  FEW_TESTS;
  std::string s;
  appendInt(s, 1234);
  ASSERT_STREQ("1234", s.c_str());
  appendInt(s, 0xab12, "%x");
  ASSERT_STREQ("1234ab12", s.c_str());
}
TEST(StringToolTest, appendCString) {
  FEW_TESTS;
  char buffer[20] = {0};
  size_t length = 0;
  ASSERT_STREQ("abc", appendCString(buffer, length, sizeof buffer, "abc"));
  ASSERT_EQ(length, 3);
  ASSERT_STREQ("abc12", appendCString(buffer, length, sizeof buffer, "123", 2));
  ASSERT_EQ(length, 5);
}
TEST(StringToolTest, copyNCString) {
  FEW_TESTS;
  char buffer[20] = {0};
  ASSERT_STREQ("012345678", copyNCString(buffer, 10, "01234567890"));
  ASSERT_STREQ("abc", copyNCString(buffer, 10, "abcd", 3));
  ASSERT_STREQ("AB", copyNCString(buffer, 3, "ABCDE", 3));
}
TEST(StringToolTest, isFloat) {
  FEW_TESTS;
  ASSERT_TRUE(isFloat("+12.34E+77"));
  ASSERT_TRUE(isFloat("-12.34E-77"));
  ASSERT_TRUE(isFloat("12.34E77"));
  ASSERT_TRUE(isFloat("12."));
  ASSERT_TRUE(isFloat("0"));
  ASSERT_TRUE(isFloat("12"));
  ASSERT_TRUE(isFloat("0.34"));
  ASSERT_TRUE(isFloat(".34"));
  ASSERT_FALSE(isFloat("m1.34"));
  ASSERT_FALSE(isFloat(""));
  ASSERT_FALSE(isFloat("abc"));
  ASSERT_FALSE(isFloat("-3.x"));
  ASSERT_FALSE(isFloat("3x"));
}
TEST(StringToolTest, isInt) {
  FEW_TESTS;
  ASSERT_TRUE(isInt("+1234567890"));
  ASSERT_TRUE(isInt("-1234567890"));
  ASSERT_TRUE(isInt("874321"));
  ASSERT_TRUE(isInt("0"));
  ASSERT_FALSE(isInt("3x"));
  ASSERT_FALSE(isInt("y33"));
  ASSERT_FALSE(isInt(""));
}
TEST(StringToolTest, isNat) {
  FEW_TESTS;
  ASSERT_FALSE(isNat("+874321"));
  ASSERT_FALSE(isNat("-874321"));
  ASSERT_TRUE(isNat("874321"));
  ASSERT_TRUE(isNat("0"));
  ASSERT_FALSE(isNat("3x"));
  ASSERT_FALSE(isNat("y33"));
  ASSERT_FALSE(isNat(""));
}
TEST(StringToolTest, isBool) {
  FEW_TESTS;
  bool value;
  for (int ix = 1; ix <= 4; ix++) {
    value = false;
    ASSERT_TRUE(isBool("True", ix, &value));
    ASSERT_TRUE(value);
  }
  for (int ix = 1; ix <= 5; ix++) {
    value = true;
    ASSERT_TRUE(isBool("False", ix, &value));
    ASSERT_FALSE(value);
  }
  ASSERT_FALSE(isBool("0"));
  ASSERT_FALSE(isBool("trues"));
  ASSERT_FALSE(isBool("falses"));
}
TEST(StringToolTest, isSize) {
  FEW_TESTS;
  int64_t value;
  ASSERT_TRUE(isSize("1234", -1, &value));
  ASSERT_EQ(1234.0, value);
  ASSERT_TRUE(isSize("321B", -1, &value));
  ASSERT_EQ(321.0, value);
  ASSERT_TRUE(isSize("99Byte", -1, &value));
  ASSERT_EQ(99.0, value);
  ASSERT_TRUE(isSize("99k", -1, &value));
  ASSERT_EQ(99E3, value);
  ASSERT_TRUE(isSize("21M", -1, &value));
  ASSERT_EQ(21E6, value);
  ASSERT_TRUE(isSize("4G", -1, &value));
  ASSERT_EQ(4E9, value);
  ASSERT_TRUE(isSize("99T", -1, &value));
  ASSERT_EQ(99E12, value);
  ASSERT_TRUE(isSize("99ki", -1, &value));
  ASSERT_EQ(99.0 * 1024, value);
  ASSERT_TRUE(isSize("21MI", -1, &value));
  ASSERT_EQ(21.0 * 1024 * 1024, value);
  ASSERT_TRUE(isSize("4GI", -1, &value));
  ASSERT_EQ(4.0 * 1024 * 1024 * 1024, value);
  ASSERT_TRUE(isSize("99TI", -1, &value));
  ASSERT_EQ(99.0 * 1024 * 1024 * 1024 * 1024, value);
  ASSERT_TRUE(isSize("99kibyte", -1, &value));
  ASSERT_EQ(99.0 * 1024, value);
  ASSERT_TRUE(isSize("21MiBiByte", -1, &value));
  ASSERT_EQ(21.0 * 1024 * 1024, value);
  ASSERT_TRUE(isSize("4GIBiBYTE", -1, &value));
  ASSERT_EQ(4.0 * 1024 * 1024 * 1024, value);
  ASSERT_TRUE(isSize("99TIBIBYTE", -1, &value));
  ASSERT_EQ(99.0 * 1024 * 1024 * 1024 * 1024, value);
  ASSERT_FALSE(isSize("-3"));
  ASSERT_FALSE(isSize("0kiwi"));
}
TEST(StringToolTest, indexOf) {
  FEW_TESTS;
  ASSERT_EQ(0, indexOf("1234", 4, "123", 3));
  ASSERT_EQ(0, indexOf("1234", 5, "123", 3));
  ASSERT_EQ(0, indexOf("1234", 3, "123", 3));
  ASSERT_EQ(1, indexOf("abcd", 3, "bc", 2));
  ASSERT_EQ(1, indexOf("abcd", 3, "bc", 1));
  ASSERT_EQ(-1, indexOf("abcd", 3, "cd", 2));
  ASSERT_EQ(-1, indexOf("abcd", 3, "cdx", 2));
  ASSERT_EQ(-1, indexOf("abc", 4, "aBc", 3));
}
TEST(StringToolTest, indexOfStart) {
  FEW_TESTS;
  ASSERT_EQ(-1, indexOf("1234", 4, "123", 3, 1));
  ASSERT_EQ(4, indexOf("12341234", -1, "123", 3, 1));
  ASSERT_EQ(-1, indexOf("12341234", 4, "123", 3, 1));
}
TEST(StringToolTest, indexOfIgnoreCase) {
  FEW_TESTS;
  ASSERT_EQ(0, indexOf("AbCd", 4, "aBc", 3, 0, true));
  ASSERT_EQ(0, indexOf("AbCd", 5, "aBc", 3, 0, true));
  ASSERT_EQ(0, indexOf("aBCd", 3, "abC", 3, 0, true));
  ASSERT_EQ(1, indexOf("aBcd", 3, "bC", 2, 0, true));
  ASSERT_EQ(1, indexOf("abCd", 3, "Bc", 1, 0, true));
  ASSERT_EQ(-1, indexOf("abCD", 3, "cd", 2, 0, true));
  ASSERT_EQ(2, indexOf("abcd", 4, "CDx", 2, 0, true));
}
TEST(StringToolTest, indexOfStartIgnoreCaseStart) {
  FEW_TESTS;
  ASSERT_EQ(-1, indexOf("aBcD", 4, "AbC", 3, 1, true));
  ASSERT_EQ(4, indexOf("AbCdAbCd", -1, "abc", 3, 1, true));
  ASSERT_EQ(-1, indexOf("AbCdAbCd", 4, "AbC", 3, 1, true));
}
TEST(StringToolTest, setString) {
  FEW_TESTS;
  std::string s;
  ASSERT_STREQ("AbCd", setString(s, "AbCd").c_str());
  ASSERT_STREQ("AbCd", s.c_str());
  ASSERT_STREQ("AbC", setString(s, "AbCd", 3).c_str());
  ASSERT_STREQ("", setString(s, "AbCd", 0).c_str());
  ASSERT_STREQ("", s.c_str());
  ASSERT_STREQ("12", setString(s, "123", 2).c_str());
  ASSERT_STREQ("", setString(s, "").c_str());
}

TEST(StringToolTest, countCString) {
  FEW_TESTS;
  std::string s;
  ASSERT_EQ(3, countCString(" aaa ", -1, "a"));
  ASSERT_EQ(3, countCString("aaa", -1, "a"));
  ASSERT_EQ(3, countCString("axabba", -1, "a"));
  ASSERT_EQ(1, countCString("abba baab", -1, "bb"));
  ASSERT_EQ(3, countCString("xxxxxx", -1, "xx"));
  ASSERT_EQ(3, countCString("xxxxaxx", -1, "xx"));
  ASSERT_EQ(2, countCString(" aaa ", 3, "a"));
  ASSERT_EQ(2, countCString(" aaa ", 3, "ab", 1));
  ASSERT_EQ(0, countCString("abc", 3, ""));
  ASSERT_EQ(0, countCString("abc", 3, "a", 0));
}
TEST(StringToolTest, trimString) {
  FEW_TESTS;
  std::string s;
  s = "  abc ";
  ASSERT_STREQ("abc", trimString(s).c_str());
  s = "\t\n\v\r abc ";
  ASSERT_STREQ("abc ", trimString(s, true, false).c_str());
  s = " abc \t\n\v\r";
  ASSERT_STREQ(" abc", trimString(s, false, true).c_str());
  s = "xxabcxxx";
  ASSERT_STREQ("abc", trimString(s, true, true, "xy").c_str());
}
TEST(StringToolTest, replaceString) {
  FEW_TESTS;
  std::string s = "aababcc";
  replaceString(s, "ab", "xyz");
  ASSERT_STREQ("axyzxyzcc", replaceString(s, "ab", "xyz").c_str());
  ASSERT_STREQ("axyzxyzcc", s.c_str());
  ASSERT_STREQ("axyzxyz1", replaceString(s, "cc", "1").c_str());
  ASSERT_STREQ("aXzxyz1", replaceString(s, "xy", "X", 1).c_str());
}
TEST(StringToolTest, countCharInCString) {
  FEW_TESTS;
  std::string s = "aababcc";
  ASSERT_EQ(3, countCharInCString("aaa", 'a'));
  ASSERT_EQ(0, countCharInCString("aaa", 'A'));
  ASSERT_EQ(2, countCharInCString(".a.", '.'));
  ASSERT_EQ(1, countCharInCString(".a.", 'a'));
}
TEST(StringToolTest, splitString) {
  FEW_TESTS;
  std::string s = "1   2\t\t3\n4";
  const std::regex &separator = stringToolRegexWhitespaces;
  auto v = splitString(s, separator);
  ASSERT_EQ(4, v.size());
  ASSERT_STREQ("1", v[0].c_str());
  ASSERT_STREQ("4", v[3].c_str());
}

TEST(StringToolTest, globToRegularExpression) {
  FEW_TESTS;
  ASSERT_STREQ(".*\\.reg .\\..*", globToRegularExpression("*.reg ?.*").c_str());
  ASSERT_STREQ("[*(){}.\\\\]\\(\\)\\{\\}\\.\\\\", globToRegularExpression("[*(){}.\\\\](){}.\\").c_str());
}

TEST(StringToolTest, stringToRegularExpression) {
  FEW_TESTS;
  ASSERT_STREQ(R"""(\(\)\[\]\{\}\+\-\*\^\$)""", stringToRegularExpression("()[]{}+-*^$").c_str());
  std::regex regExpr(stringToRegularExpression("^()[]{}+-*$"));
  ASSERT_TRUE(std::regex_match("^()[]{}+-*$", regExpr));
  std::regex regExpr2(stringToRegularExpression("^)(]abc[}{+-$*"));
  ASSERT_TRUE(std::regex_match("^)(]abc[}{+-$*", regExpr2));
}

TEST(StringToolTest, stringIndex) {
  FEW_TESTS;
  auto list = splitCString("alice bobby bob charly", " ");
  ASSERT_EQ(2, indexOf(list, "bobx", 3));
  ASSERT_EQ(1, indexOf(list, "bobby"));
  ASSERT_EQ(-1, indexOf(list, "bobb"));
  auto list2 = splitCString("a,bc,def,e12", ",");
  ASSERT_EQ(0, indexOf(list2, "a", 1, true));
  ASSERT_EQ(1, indexOf(list2, "bc", -1, true));
  ASSERT_EQ(2, indexOf(list2, "def", 3, true));
  ASSERT_EQ(3, indexOf(list2, "e12", -1, true));
  ASSERT_EQ(-1, indexOf(list2, "e13", -1, true));
}

TEST(StringToolTest, crc32) {
  FEW_TESTS;
  auto data = "With a little help from my friends";
  ASSERT_EQ(0x3DBC3A44, crc32((uint8_t*) data, strlen(data)));
  data = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  ASSERT_EQ(0x8F0A1199, crc32((uint8_t*) data, strlen(data)));
  data = "1234";
  ASSERT_EQ(0x9be3e0a3, crc32((uint8_t*) data, strlen(data)));
  data = "1235";
  ASSERT_EQ(0xece4d035, crc32((uint8_t*) data, strlen(data)));
  data = "With a little help";
  auto crc = crc32((uint8_t*) data, strlen(data), false);
  data = " from my friends";
  ASSERT_EQ(0x3DBC3A44, crc32Update((uint8_t*) data, strlen(data), crc, true));
}

TEST(StringToolTest, escapeMetaCharacters) {
  FEW_TESTS;
  std::string x("2slash: \\\\ Apo:\" NL: \n CR: \r TAB: \t VTAB: \v ONE: \x01");
  const char* expected = "2slash: \\\\\\\\ Apo:\\\" NL: \\n CR: \\r TAB: \\t VTAB: \\v ONE: \\x01";
  ASSERT_STREQ(expected, escapeMetaCharacters(x.c_str()).c_str());
  auto y = escapeMetaCharacters(x);
  ASSERT_STREQ(expected, x.c_str());
  ASSERT_STREQ(expected, y.c_str());
}

TEST(StringToolTest, escapeMetaCharactersCount) {
  //FEW_TESTS;
  const char* x = "2slash: \\\\ Apo:\" NL: \n CR: \r TAB: \t VTAB: \v ONE: \x01";
  ASSERT_EQ(escapeMetaCharactersCount(x), 7+3);
  const char* a = "\x05";
  ASSERT_EQ(escapeMetaCharactersCount(a), 3);
}

