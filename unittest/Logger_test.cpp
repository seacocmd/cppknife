/*
 * Logger_test.cpp
 *
 *  Created on: 03.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../basic/basic.hpp"

using namespace cppknife;

TEST(LoggerTest, basics) {
  Logger logger;
  logger.setLevel(LV_INFO);
  logger.addAppender(new MemoryAppender(10));
  logger.log("Hi");
  Appender *appender = logger.findAppender("memory");
  ASSERT_TRUE(appender != nullptr);
  MemoryAppender *appender2 = dynamic_cast<MemoryAppender*>(appender);
  ASSERT_EQ(appender2->count(), 1);
  ASSERT_EQ(appender2->at(0), "Hi");
}
