/*
 * fileknife.cpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "../unittest/google_test.hpp"
#include "sesknife.hpp"

using namespace cppknife;

int main(int argc, char **argv) {
  int rc = EXIT_SUCCESS;
  const char *arg1 = argc < 2 ? "" : argv[1];
  if (argc > 1 && strcmp(arg1, "--test") == 0) {
    const char *filter = "*";
    //filter = "ArgumentParserTest.*";
    //filter = "BaseRandomTest.*";
    //filter = "ByteStorageTest.*";
    //filter = "CsvFileTest.*";
    //filter = "ConfigurationTest.*";
    //filter = "FileTest.*";
    //filter = "FileToolTest.*";
    //filter = "JsonReaderTest.*";
    //filter = "LinesStreamTest.*";
    //filter = "LineReaderTest.*";
    //filter = "LineListTest.*";
    //filter = "LoggerTest.*";
    //filter = "MatcherTest.*";
    //filter = "ParserTest.*";
    //filter = "PathTest.*";
    //filter = "SesKnifeTest.*";
    //filter = "StorageTest.*";
    //filter = "StringListTest.*";
    //filter = "StringToolTest.*";
    //filter = "TimeToolTest.*";
    //filter = "TraverserTest.*";
    //filter = "TextKnifeTest.*";
    //filter = "FunctionEngineTest.*";
    //filter = "SearchEngineTest.*";
    filter = "ScriptTest.*";
    //filter = "SesKnifeTest.*";
    std::string arg = "--gtest_filter=";
    arg += filter;
    char *args[] = { (char*) "dummy", const_cast<char*>(arg.c_str()), nullptr };
    googleTest(2, args);
  } else {
    rc = searchEngineScriptKnife(argc - 1, argv + 1);
  }
  return rc;
}

