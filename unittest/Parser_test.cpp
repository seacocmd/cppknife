/*
 * Parser_test.cpp
 *
 *  Created on: 03.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../text/text.hpp"
using namespace cppknife;

static bool fewTests() {
  return true;
}
#define FEW_TESTS if (fewTests()) return

static const std::vector<std::string> keywords( { "else", "fi", "if" });

TEST(ParserTest, basic) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  Parser parser(*logger);
  parser.setKeywords(keywords);
  parser.setInput(" if ( 1 < 3.0  ) _word fi");
  ASSERT_EQ(TT_KEYWORD, parser.parse());
  ASSERT_STREQ("if", parser.tokenAsCString());
  ASSERT_EQ(2, parser.token()._keywordId);
  ASSERT_EQ(TT_OPERATOR, parser.parse());
  ASSERT_STREQ("(", parser.tokenAsCString());
  ASSERT_EQ(TT_NUMBER, parser.lookAhead());
  ASSERT_EQ(TT_NUMBER, parser.parse());
  ASSERT_STREQ("1", parser.tokenAsCString());
  ASSERT_EQ(TT_OPERATOR, parser.parse());
  ASSERT_STREQ("<", parser.tokenAsCString());
  ASSERT_EQ(TT_NUMBER, parser.parse());
  ASSERT_STREQ("3.0", parser.tokenAsCString());
  ASSERT_EQ(TT_OPERATOR, parser.lookAhead());
  ASSERT_STREQ(")", parser.nextToken()._string.c_str());
  ASSERT_EQ(TT_OPERATOR, parser.parse());
  ASSERT_STREQ(")", parser.tokenAsCString());
  ASSERT_EQ(TT_IDENTIFIER, parser.parse());
  ASSERT_STREQ("_word", parser.tokenAsCString());
  ASSERT_EQ(TT_KEYWORD, parser.parse());
  ASSERT_EQ(1, parser.token()._keywordId);
  ASSERT_STREQ("fi", parser.tokenAsCString());
  ASSERT_EQ(TT_EOF, parser.parse());
  ASSERT_EQ(TT_EOF, parser.parse());
  delete logger;
}

TEST(ParserTest, parseSpecial) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  const std::regex reVariable("\\$\\([_A-Za-z]\\w+\\)");
  const std::regex reString("([^\\w]).*\\1");
  Parser parser(*logger);
  parser.addSpecialTokenRegex(reVariable);
  parser.addSpecialTokenRegex(reString);
  parser.setInput(" $(abc) /def/ what");
  ASSERT_EQ(TT_SPECIAL_1, parser.parseSpecial());
  ASSERT_EQ(TT_SPECIAL_2, parser.parseSpecial());
  ASSERT_EQ(TT_UNKNOWN, parser.parseSpecial());
  delete logger;
}

TEST(ParserTest, commentOneLine) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  Parser parser(*logger);
  parser.setInput(" # comment\n123");
  ASSERT_EQ(TT_COMMENT, parser.parse());
  ASSERT_EQ(TT_NUMBER, parser.parse());
  ASSERT_EQ(TT_EOF, parser.parse());
  delete logger;
}

TEST(ParserTest, tokenAsInterpolatedString) {
  //FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  Parser parser(*logger);
  parser.setInput("'\\t\\r\\n\\v\\x20\\x9!'");
  ASSERT_EQ(TT_STRING, parser.parse());
  auto value = parser.tokenAsInterpolatedString();
  ASSERT_STREQ("\t\r\n\v \t!", value.c_str());
  ASSERT_EQ(TT_EOF, parser.parse());
  delete logger;
}

