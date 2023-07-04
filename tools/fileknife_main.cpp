/*
 * fileknife.cpp
 *
 *  Created on: 09.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "../unittest/google_test.hpp"
#include "fileknife.hpp"

using namespace cppknife;

int main(int argc, char **argv) {
  const char *arg1 = argc < 2 ? "" : argv[1];
  if (argc > 1 && strcmp(arg1, "--test") == 0) {
    const char *filter = "*";
    //filter = "ArgumentParserTest.*";
    //filter = "BaseRandomTest.*";
    //filter = "ByteStorageTest.*";
    filter = "FileKnifeTest.*";
    //filter = "FileTest.*";
    //filter = "FileToolTest.*";
    //filter = "LineAgentTest.*";
    //filter = "LineListTest.*";
    //filter = "LoggerTest.*";
    //filter = "MatcherTest.*";
    //filter = "PathTest.*";
    //filter = "StorageTest.*";
    //filter = "StringListTest.*";
    //filter = "StringToolTest.*";
    //filter = "TimeToolTest.*";
    //filter = "TraverserTest.*";
    std::string arg = "--gtest_filter=";
    arg += filter;
    char *args[] = { (char*) "dummy", const_cast<char*>(arg.c_str()), nullptr };
    googleTest(2, args);
  } else {
    fileKnife(argc - 1, argv + 1);
  }
  return EXIT_SUCCESS;
}

