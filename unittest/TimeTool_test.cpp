/*
 * TimeTool_test.cpp
 *
 *  Created on: 06.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
using namespace cppknife;

TEST(TimeToolTest, timeDifferenceToString) {
  double seconds = 3.0 * 86400 + 2 * 3600 + 43 * 60 + 10.123456;
  ASSERT_STREQ("3:02:43:10.123", timeDifferenceToString(seconds).c_str());
  ASSERT_STREQ("3d02h43m10.123s",
      timeDifferenceToString(seconds, "%dd%hh%mm%s.%3s").c_str());
  ASSERT_STREQ("74:43:10.123s",
      timeDifferenceToString(seconds, "%h:%m:%s.%3s").c_str());
  ASSERT_STREQ("4483:10.123456s",
      timeDifferenceToString(seconds, "%m:%s.%6s").c_str());
}
TEST(TimeToolTest, nowAsDouble) {
  double now1 = nowAsDouble();
  double now2 = static_cast<double>(time(nullptr));
  ASSERT_TRUE(fabs(now2 - now1) < 1.0);
}

TEST(TimeToolTest, formatDate) {
  std::time_t time1 = 123456;

  ASSERT_STREQ("1970.01.02", formatDate(time1).c_str());
  ASSERT_STREQ("02-01-1970", formatDate(time1, false, "-").c_str());
  auto date(formatDate());
  ASSERT_TRUE(date[0] == '2');
}
TEST(TimeToolTest, formatTime) {
  std::time_t time1 = 1732;

  ASSERT_STREQ("01:28:52", formatTime(time1).c_str());
  ASSERT_STREQ("01+28", formatTime(time1, false, "+").c_str());
  auto date(formatDate());
  ASSERT_TRUE(date[0] == '2');
}
TEST(TimeToolTest, formatDateTime) {
  std::time_t time1 = 123456;

  ASSERT_STREQ("1970.01.02 11:17:36", formatDateTime(time1).c_str());
  ASSERT_STREQ("02-01-1970/11+17",
      formatDateTime(time1, false, "/", false, "-", "+").c_str());
  auto datetime(formatDateTime());
  ASSERT_TRUE(datetime[0] == '2');
}
