/*
 * JsonReader_test.cpp
 *
 *  Created on: 10.04.2022
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

TEST(JsonReaderTest, basic) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_DEBUG);
  auto fnSource = temporaryFile("test1.json", "unittest", true);
  writeText(fnSource.c_str(),
      R"""({
"number": 10.5,
"string": "hello",
"bool": true,
"array": [ 1, 2, 3],
"map": { "a": 47, "b": "xyz" }
}
)""");
  FileLinesStream stream(fnSource.c_str(), *logger);
  JsonReader reader(*logger);
  ASSERT_TRUE(reader.parse(stream));
  ASSERT_EQ(10.5, reader.map("number").asDouble());
  ASSERT_STREQ("hello", reader.map("string").asString());
  ASSERT_TRUE(reader.map("bool").asBool());
  ASSERT_EQ(1, reader.map("array").array()[0].asInt());
  ASSERT_EQ(2, reader.map("array").array()[1].asInt());
  ASSERT_EQ(3, reader.map("array").array()[2].asInt());
  ASSERT_EQ(47, reader.map("map").map("a").asInt());
  ASSERT_STREQ("xyz", reader.map("map").map("b").asString());
  delete logger;
}
}

