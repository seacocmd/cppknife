/*
 * InternalError_test.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;
TEST(InternalErrorTest, basics) {
  try {
    throw InternalError("Hi");
    ASSERT_FALSE(true);
  } catch (const InternalError &e) {
    ASSERT_STREQ(e.message(), "Hi");
  }
}
