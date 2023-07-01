/*
 * StringList_test.cpp
 *
 *  Created on: 12.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
using namespace cppknife;
TEST(StringListTest, append) {
  StringList list(1, 2);
  list.append("Hi");
  std::string s("world");
  list.append(s);
  list.append("and");
  list.append("the");
  list.append("rest!");
  ASSERT_EQ(5, list.length());
  std::string all = list.join(" ");
  ASSERT_STREQ("Hi world and the rest!", all.c_str());
}
TEST(StringListTest, at) {
  StringList list;
  ASSERT_STREQ("", list.at(-1));
  ASSERT_STREQ("", list.at(0));
  list.append("Hi");
  ASSERT_STREQ("", list.at(-1));
  ASSERT_STREQ("Hi", list.at(0));
  ASSERT_STREQ("", list.at(1));
  list.append(" ");
  list.append("world");
  std::string all;
  for (size_t ix = 0; ix < list.length(); ix++) {
    all.append(list.at(ix));
  }
  ASSERT_STREQ("Hi world", all.c_str());
}
TEST(StringListTest, clear) {
  StringList list;
  list.append("Hi");
  ASSERT_EQ(1, list.length());
  list.clear();
  ASSERT_EQ(0, list.length());
}
TEST(StringListTest, join) {
  StringList list;
  list.append("Hi");
  list.append("great");
  ASSERT_STREQ("Higreat", list.join().c_str());
  ASSERT_STREQ("Hi\ngreat", list.join("\n").c_str());
  list.append("world");
  ASSERT_STREQ("Higreatworld", list.join().c_str());
  ASSERT_STREQ("Hi\ngreat\nworld", list.join("\n").c_str());
}

