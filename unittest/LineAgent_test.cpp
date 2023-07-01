/*
 * LineAgent_test.cpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;

static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

TEST(LineAgentTest, basics) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn = theOsInfo._tempDirectorySeparator + "lineagent.test.txt";
  writeText(fn.c_str(), R"(1
abc
ABCDEFGH
)");
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  LineAgent agent(logger);
  ASSERT_TRUE(agent.openFile(fn.c_str()));
  size_t length = 0;
  ASSERT_STREQ("1", agent.nextLine(length));
  ASSERT_EQ(1, length);
  ASSERT_STREQ("abc", agent.nextLine(length));
  ASSERT_EQ(3, length);
  ASSERT_STREQ("ABCDEFGH", agent.nextLine(length));
  ASSERT_EQ(8, length);
  ASSERT_EQ(nullptr, agent.nextLine(length));
  ASSERT_EQ(0, length);
  ASSERT_EQ(nullptr, agent.nextLine(length));
  ASSERT_EQ(0, length);
  delete logger;
}
TEST(LineAgentTest, multiLength) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn = theOsInfo._tempDirectorySeparator + "lineagent.test.txt";
  writeText(fn.c_str(), R"(1
abc
ABCD
one
Tx
.
)");
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  LineAgent agent(logger);
  size_t length = 0;
  for (auto size = 4; size < 16; size++) {
    ASSERT_TRUE(agent.openFile(fn.c_str()));
    agent.setBufferSize(size);
    ASSERT_STREQ("1", agent.nextLine(length));
    ASSERT_EQ(1, length);
    ASSERT_STREQ("abc", agent.nextLine(length));
    ASSERT_EQ(3, length);
    ASSERT_STREQ("ABCD", agent.nextLine(length));
    ASSERT_EQ(4, length);
    if (size == 4) {
      ASSERT_STREQ("", agent.nextLine(length));
      ASSERT_EQ(0, length);
    }
    ASSERT_STREQ("one", agent.nextLine(length));
    ASSERT_EQ(3, length);
    if (size == 6) {
      size = 6;
    }
    ASSERT_STREQ("Tx", agent.nextLine(length));
    ASSERT_EQ(2, length);
    ASSERT_STREQ(".", agent.nextLine(length));
    ASSERT_EQ(1, length);
    ASSERT_EQ(nullptr, agent.nextLine(length));
    ASSERT_TRUE(agent.eof());
    ASSERT_FALSE(agent.hasBinaryData());
  }
  delete logger;
}
TEST(LineAgentTest, binaryData) {
  FEW_TESTS;
  auto theOsInfo(osInfo());
  auto fn = theOsInfo._tempDirectorySeparator + "lineagent.test.txt";
  char data[] = {'a', 'b', 'c', '\n', '1', '2', '\n', 1, 2};
  writeBinary(fn.c_str(), data, sizeof data);
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  LineAgent agent(logger);
  agent.setBufferSize(4);
  agent.openFile(fn.c_str());
  size_t length;
  ASSERT_STREQ("abc", agent.nextLine(length));
  ASSERT_EQ(3, length);
  ASSERT_FALSE(agent.hasBinaryData());
  ASSERT_STREQ("12", agent.nextLine(length));
  ASSERT_EQ(2, length);
  ASSERT_FALSE(agent.hasBinaryData());
  ASSERT_STREQ("\001\002", agent.nextLine(length));
  ASSERT_TRUE(agent.hasBinaryData());
  delete logger;
}

