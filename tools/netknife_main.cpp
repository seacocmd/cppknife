/*
 * netknife.cpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */
#include "netknife.hpp"
#include "../unittest/google_test.hpp"

using namespace cppknife;

int main(int argc, char **argv) {
  const char *arg1 = argc < 2 ? "" : argv[1];
  if (argc > 1 && strcmp(arg1, "--test") == 0) {
    const char *filter = "*";
    filter = "SocketServerTest.*";
    std::string arg = "--gtest_filter=";
    arg += filter;
    char *args[] = { (char*) "dummy", const_cast<char*>(arg.c_str()), nullptr };
    googleTest(2, args);
  } else {
    netknife(argc - 1, argv + 1);
  }
  return EXIT_SUCCESS;
}

