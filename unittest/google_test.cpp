/*
 * google_test.cpp
 *
 *  Created on: 09.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "google_test.hpp"

int googleTest(int argc, char **argv) {
  testing::InitGoogleTest (&argc, argv);
  return RUN_ALL_TESTS ();
}
