/*
 * LineReader_test.cpp
 *
 *  Created on: 11.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;

static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

TEST(LineReaderTest, basics) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  writeText(fnSource.c_str(), R"(1
abc
ABCDEFGH
)");
  LineReader reader(fnSource.c_str(), *logger, true, 2);
  ASSERT_FALSE(reader.endOfFile());
  ASSERT_STREQ("1", reader.nextLine().c_str());
  ASSERT_FALSE(reader.endOfFile());
  ASSERT_STREQ("abc", reader.nextLine().c_str());
  ASSERT_FALSE(reader.endOfFile());
  ASSERT_STREQ("ABCDEFGH", reader.nextLine().c_str());
  ASSERT_TRUE(reader.endOfFile());
  ASSERT_STREQ("", reader.nextLine().c_str());
  ASSERT_TRUE(reader.endOfFile());
  delete logger;
}

TEST(LineReaderTest, multipleLengths) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  writeText(fnSource.c_str(), R"(1
abc
ABCD
one
Tx
.
)");
  LineReader reader(nullptr, *logger, true, 2);
  for (auto size = 2; size < 16; size++) {
    //logger->say(LV_INFO, formatCString("size: %d", size));
    ASSERT_TRUE(reader.openFile(fnSource.c_str()));
    reader.setBufferSize(size);
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("1", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("abc", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("ABCD", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("one", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("Tx", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ(".", reader.nextLine().c_str());
    ASSERT_TRUE(reader.endOfFile());
    ASSERT_STREQ("", reader.nextLine().c_str());
    ASSERT_TRUE(reader.endOfFile());
    //logger->log(formatCString("size: %d block reads: %d", size, reader._countBlockReads));
    reader._countBlockReads = 0;
  }
  delete logger;
}
TEST(LineReaderTest, multipleLengthsCR) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  writeText(fnSource.c_str(), "1\r\nabc\r\nABCD\r\none\r\nTx\r\n.\r\n");
  LineReader reader(nullptr, *logger, true, 2);
  for (auto size = 2; size < 16; size++) {
    //logger->say(LV_INFO, formatCString("size: %d", size));
    ASSERT_TRUE(reader.openFile(fnSource.c_str()));
    reader.setBufferSize(size);
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("1", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("abc", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("ABCD", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("one", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ("Tx", reader.nextLine().c_str());
    ASSERT_FALSE(reader.endOfFile());
    ASSERT_STREQ(".", reader.nextLine().c_str());
    ASSERT_TRUE(reader.endOfFile());
    ASSERT_STREQ("", reader.nextLine().c_str());
    ASSERT_TRUE(reader.endOfFile());
    //logger->log(formatCString("size: %d block reads: %d", size, reader._countBlockReads));
    reader._countBlockReads = 0;
  }
  delete logger;
}

TEST(LineReaderTest, lookahead) {
  //FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  writeText(fnSource.c_str(), R"(1
abc
ABCDEFGH
)");
  LineReader reader(fnSource.c_str(), *logger, true, 2);
  auto lines = reader.lookahead(2);
  ASSERT_EQ(2, lines.size());
  ASSERT_STREQ("1", lines[0].c_str());
  ASSERT_STREQ("abc", lines[1].c_str());
  ASSERT_FALSE(reader.endOfFile());
  ASSERT_STREQ("1", reader.nextLine().c_str());
  ASSERT_FALSE(reader.endOfFile());
  ASSERT_STREQ("abc", reader.nextLine().c_str());
  ASSERT_FALSE(reader.endOfFile());
  ASSERT_STREQ("ABCDEFGH", reader.nextLine().c_str());
  ASSERT_TRUE(reader.endOfFile());
}

TEST(LineReaderTest, binaryData) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  char data[] = {'a', 'b', 'c', '\n', '1', '2', '\n', 1, 2};
  writeBinary(fnSource.c_str(), data, sizeof data);
  LineReader reader(fnSource.c_str(), *logger, true, 4);
  ASSERT_FALSE(reader.hasBinaryData(true));
  ASSERT_STREQ("abc", reader.nextLine().c_str());
  ASSERT_TRUE(reader.hasBinaryData());
  ASSERT_STREQ("12", reader.nextLine().c_str());
  ASSERT_STREQ("\001\002", reader.nextLine().c_str());
  ASSERT_TRUE(reader.hasBinaryData(false));
  delete logger;
}

TEST(LineReaderTest, binaryDataNull) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  writeText(fnSource.c_str(), R"(1234
abcd
ABCD
1234
abcd
ABCD
)");
  LineReader reader(nullptr, *logger, true, 10);
  reader.openFile(fnSource.c_str());
  ASSERT_EQ(9, reader.estimateLineCount());
  delete logger;
}

TEST(LineReaderTest, estimateLineCount) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("linereader.txt", "unittest", true);
  char data[] = {'\0', 'b', '\0', '\n'};
  std::string data2;
  data2.resize(sizeof data + 1);
  memcpy(data2.data(), data, sizeof data);
  writeBinary(fnSource.c_str(), data, sizeof data);
  LineReader reader(fnSource.c_str(), *logger);
  ASSERT_TRUE(reader.hasBinaryData());
  ASSERT_TRUE(memcmp(&data2[0], reader.nextLine().c_str(), 4) == 0);
  delete logger;
}

