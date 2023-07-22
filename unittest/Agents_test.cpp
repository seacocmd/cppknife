/*
 * OsException_test.cpp
 *
 *  Created on: 21.07.2023
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../net/net.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(AgentsTest, storagePut)
{
  //FEW_TESTS;
  auto logger = buildMemoryLogger();

  KniveSocketClient client("/run/cppknife/netknife.knife", *logger);
  auto answer = client.send(StorageJobAgent::SCOPE_STORAGE,
      StorageJobAgent::JOB_PUT, "mything\nis known");
  ASSERT_TRUE(answer);

}
