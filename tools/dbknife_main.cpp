/*
 * fileknife.cpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "../unittest/google_test.hpp"
#include "dbknife.hpp"
using namespace cppknife;

int main(int argc, char **argv) {
  const char *arg1 = argc < 2 ? "" : argv[1];
  if (argc > 1 && strcmp(arg1, "--test") == 0) {
    const char *filter = "*";
    //filter = "ArgumentParserTest.*";
    //filter = "BaseRandomTest.*";
    //filter = "ByteStorageTest.*";
    //filter = "CharRandomTest.*";
    //filter = "ConfigurationTest.*";
    //filter = "FileTest.*";
    //filter = "FileToolTest.*";
    //filter = "KissRandomTest.*";
    //filter = "LineAgentTest.*";
    //filter = "LineListTest.*";
    //filter = "LoggerTest.*";
    //filter = "MatcherTest.*";
    //filter = "MySqlTest.*";
    //filter = "PathTest.*";
    //filter = "PostgresTest.*";
    filter = "ProcessTest.*";
    //filter = "PortableRandomTest.*";
    //filter = "SecretConfigurationTest.*";
    //filter = "StorageTest.*";
    //filter = "StringListTest.*";
    //filter = "StringToolTest.*";
    //filter = "TimeToolTest.*";
    //filter = "TraverserTest.*";
    //filter = "VeilEngineTest.*";
    //filter = "DbKnifeTest.*";
    std::string arg = "--gtest_filter=";
    arg += filter;
    char *args[] = { (char*) "dummy", const_cast<char*>(arg.c_str()), nullptr };
    googleTest(2, args);
  } else {
    dbknife(argc - 1, argv + 1);
  }
  return EXIT_SUCCESS;
}

