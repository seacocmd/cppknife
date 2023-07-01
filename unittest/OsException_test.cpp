/*
 * OsException_test.cpp
 *
 *  Created on: 15.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;
TEST(OsException, basic)
{
  try {
      throw OsException("Hi");
      ASSERT_TRUE(false);
  } catch (OsException &e) {
      ASSERT_STREQ(e.message(), "Hi");
  }
}
