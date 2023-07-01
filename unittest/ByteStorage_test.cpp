/*
 * ByteStorage_test.cpp
 *
 *  Created on: 06.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;
TEST(ByteStorageTest, basics) {
  // We need a block because of the destructor.
  if (strlen("a") == 1) {
    ByteStorage storage(100);
    auto data = storage.allocate(10);
    memset(data, 'x', 10);
    ASSERT_STREQ(data, "xxxxxxxxxx");
    ASSERT_EQ(10, storage.sizeOf(data));
    char *buffer[20];
    for (int ix = 0; ix < 15; ix++) {
      size_t length = 2 + ix;
      buffer[ix] = storage.allocate(length);
      memset(buffer[ix], 'A' + ix, length);
    }
    size_t buffers = 0, used = 0, vasted = 0;
    storage.statistics(buffers, used, vasted);
    ASSERT_EQ(2, buffers);
    ASSERT_EQ(193, used);
    ASSERT_EQ(6, vasted);
    for (int ix = 0; ix < 15; ix++) {
      ASSERT_EQ(buffer[ix][0], 'A' + ix);
      ASSERT_EQ(2 + ix, storage.sizeOf(buffer[ix]));
    }
    ASSERT_TRUE(storage.check().empty());
    storage.clear();
    storage.statistics(buffers, used, vasted);
    ASSERT_EQ(0, buffers);
    ASSERT_EQ(0, used);
    ASSERT_EQ(0, vasted);
    data = storage.allocate(10);
    memset(data, 'x', 10);
    ASSERT_STREQ(data, "xxxxxxxxxx");
    ASSERT_EQ(10, storage.sizeOf(data));
    storage.statistics(buffers, used, vasted);
    ASSERT_EQ(1, buffers);
    ASSERT_EQ(13, used);
    ASSERT_EQ(0, vasted);
    ASSERT_TRUE(storage.check().empty());
  }
}
