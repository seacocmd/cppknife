/*
 * CsvFile_test.cpp
 *
 *  Created on: 30.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../text/text.hpp"

#include "google_test.hpp"
namespace cppknife {

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

#if ! defined(TEST)
#define TEST(a,b) void b::a()
#endif
TEST(CsvFileTest, semicolon) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.csv", "unittest", true);
  // last line contains blanks and tabs:
  writeText(fnSource.c_str(),
      R"""(id;name;income
1;Miller Joe;1024.44
2;Smith Eva;1424.99
        
)""");
  CsvFile csv(*logger, fnSource.c_str());
  ASSERT_EQ(csv.rowCount(), 3);
  auto row = csv.cRow(0);
  ASSERT_NE(row, nullptr);
  ASSERT_STREQ("id", row->rawColumn(0));
  ASSERT_STREQ("name", row->rawColumn(1));
  ASSERT_STREQ("income", row->rawColumn(2));
  row = csv.cRow(1);
  ASSERT_NE(row, nullptr);
  ASSERT_EQ(1, row->asInt(0, -1));
  ASSERT_STREQ("Miller Joe", row->rawColumn(1));
  ASSERT_EQ(1024.44, row->asDouble(2, -1));
  row = csv.cRow(2);
  ASSERT_NE(row, nullptr);
  ASSERT_EQ(2, row->asInt(0, -1));
  ASSERT_STREQ("Smith Eva", row->rawColumn(1));
  ASSERT_EQ(1424.99, row->asDouble(2, -1));
  ASSERT_EQ(nullptr, row->rawColumn(4));
  ASSERT_EQ(nullptr, csv.cRow(3));
  row = csv.cRow(3);
  ASSERT_EQ(row, nullptr);
  delete logger;
}

TEST(CsvFileTest, commaAndDelimiter) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.csv", "unittest", true);
  writeText(fnSource.c_str(),
      R"""("id","name, firstname",'income'
"1","Miller, Joe","1024.44"
"2",'Smith, Eva','1424.99'
)""");
  CsvFile csv(*logger, fnSource.c_str());
  ASSERT_EQ(csv.rowCount(), 3);
  auto row = csv.cRow(0);
  ASSERT_NE(row, nullptr);
  ASSERT_STREQ("id", row->rawColumn(0));
  ASSERT_STREQ("name, firstname", row->rawColumn(1));
  ASSERT_STREQ("income", row->rawColumn(2));
  row = csv.cRow(1);
  ASSERT_NE(row, nullptr);
  ASSERT_EQ(1, row->asInt(0, -1));
  ASSERT_STREQ("Miller, Joe", row->rawColumn(1));
  ASSERT_EQ(1024.44, row->asDouble(2, -1));
  row = csv.cRow(2);
  ASSERT_NE(row, nullptr);
  ASSERT_EQ(2, row->asInt(0, -1));
  ASSERT_STREQ("Smith, Eva", row->rawColumn(1));
  ASSERT_EQ(1424.99, row->asDouble(2, -1));
  delete logger;
}
TEST(CsvFileTest, pureColumnDoubledDelimiters) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.csv", "unittest", true);
  writeText(fnSource.c_str(), R"""("a_""cat""",'a_''dog'''
)""");
  CsvFile csv(*logger, fnSource.c_str());
  ASSERT_EQ(csv.rowCount(), 1);
  auto row = csv.cRow(0);
  ASSERT_NE(row, nullptr);
  ASSERT_STREQ("a_\"cat\"", row->pureColumn(0).c_str());
  ASSERT_STREQ("a_'dog'", row->pureColumn(1).c_str());
  delete logger;
}
TEST(CsvFileTest, pureColumnEscChars) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.csv", "unittest", true);
  writeText(fnSource.c_str(), R"""("\n\r\v\t";"\"\'"
)""");
  CsvFile csv(*logger, fnSource.c_str(), CsvFile::AUTO_SEPARATOR,
      CsvFile::AUTO_DELIMITER, '\\');
  ASSERT_EQ(csv.rowCount(), 1);
  auto row = csv.cRow(0);
  ASSERT_NE(row, nullptr);
  auto column = row->pureColumn(0).c_str();
  ASSERT_EQ(column[0], '\n');
  ASSERT_EQ(column[1], '\r');
  ASSERT_EQ(column[2], '\v');
  ASSERT_EQ(column[3], '\t');
  ASSERT_STREQ("\"'", row->pureColumn(1).c_str());
  delete logger;
}
TEST(CsvFileTest, nextRow) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.csv", "unittest", true);
  writeText(fnSource.c_str(),
      R"""(id,name,income
1,"Bill, Joe",1234.00
4,"Wood, Ada",2341.77
)""");

  CsvFile csv(*logger);
  csv.openCsv(fnSource.c_str());
  int lineNo = 0;
  CsvRow row(csv);
  while (!csv.endOfFile()) {
    lineNo++;
    printf("line %d\n", lineNo);
    ASSERT_TRUE(csv.nextRow(row));
    ASSERT_EQ(3, row.columnCount());
    switch (lineNo) {
    case 1:
      ASSERT_STREQ("id", row.pureColumn(0).c_str());
      ASSERT_STREQ("id", row.rawColumn(0));
      ASSERT_STREQ("name", row.pureColumn(1).c_str());
      ASSERT_STREQ("name", row.rawColumn(1));
      ASSERT_STREQ("income", row.pureColumn(2).c_str());
      ASSERT_STREQ("income", row.rawColumn(2));
      break;
    case 2:
      ASSERT_EQ(1, row.asInt(0, -1));
      ASSERT_STREQ("Bill, Joe", row.pureColumn(1).c_str());
      ASSERT_STREQ("Bill, Joe", row.rawColumn(1));
      ASSERT_EQ(1234.00, row.asDouble(2, 0.0));
      break;
    case 3:
      ASSERT_EQ(4, row.asInt(0, -1));
      ASSERT_STREQ("Wood, Ada", row.pureColumn(1).c_str());
      ASSERT_STREQ("Wood, Ada", row.rawColumn(1));
      ASSERT_EQ(2341.77, row.asDouble(2, 0.0));
      break;
    default:
      ASSERT_TRUE(false);
    }
  }
  ASSERT_FALSE(csv.nextRow(row));
  delete logger;
}
TEST(CsvFileTest, withBlanks) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.csv", "unittest", true);
  writeText(fnSource.c_str(),
      R"""(id,x,y
 1 , 22 , -33.02
 2 , 22.44 , -44
)""");

  CsvFile csv(*logger);
  csv.openCsv(fnSource.c_str());
  int lineNo = 0;
  CsvRow row(csv);
  ASSERT_TRUE(csv.nextRow(row));
  ASSERT_TRUE(csv.nextRow(row));
  ASSERT_EQ(row.asInt(0, -1), 1);
  ASSERT_EQ(row.asDouble(1, -1E99), 22.0);
  ASSERT_EQ(row.asDouble(2, -1E99), -33.02);
  ASSERT_TRUE(csv.nextRow(row));
  ASSERT_EQ(row.asInt(0, -1), 2);
  ASSERT_EQ(row.asDouble(1, -1E99), 22.44);
  ASSERT_EQ(row.asDouble(2, -1E99), -44.0);
  ASSERT_FALSE(csv.nextRow(row));
  delete logger;
}
}

