/*
 * google_test.cpp
 *
 *  Created on: 09.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "google_test.hpp"
#include "cppknife_control.hpp"

int googleTest(int argc, char **argv) {
#ifndef NOT_USE_UNITTESTS
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
#else
  fprintf(stderr, "No unittest configured!\n");
  return 0;
#endif
}
