/*
 * geoknife.cpp
 *
 *  Created on: 21.08.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "geoknife.hpp"
#include "../unittest/google_test.hpp"

using namespace cppknife;

int main(int argc, char **argv) {
  const char *arg1 = argc < 2 ? "" : argv[1];
  if (argc > 1 && strcmp(arg1, "--test") == 0) {
    const char *filter = "*";
    filter = "GpsTranslatorTest.*";
    std::string arg = "--gtest_filter=";
    arg += filter;
    char *args[] = { (char*) "dummy", const_cast<char*>(arg.c_str()), nullptr };
    googleTest(2, args);
  } else {
    geoknife(argc - 1, argv + 1);
  }
  return EXIT_SUCCESS;
}

