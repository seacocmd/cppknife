/*
 * fileknife.cpp
 *
 *  Created on: 07.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "../unittest/google_test.hpp"
#include "textknife.hpp"

using namespace cppknife;

int main(int argc, char **argv) {
  const char *arg1 = argc < 2 ? "" : argv[1];
  if (argc > 1 && strcmp(arg1, "--test") == 0) {
    const char *filter = "*";
    //filter = "ArgumentParserTest.*";
    //filter = "Base64Test.*";
    //filter = "BaseRandomTest.*";
    //filter = "ByteStorageTest.*";
    //filter = "CsvFileTest.*";
    //filter = "ConfigurationTest.*";
    //filter = "FileTest.*";
    //filter = "FunctionEngineTest.*";
    //filter = "FileToolTest.*";
    //filter = "ScriptTest.*";
    filter = "NodeJsonTest.*";
    //filter = "LinesStreamTest.*";
    //filter = "LineReaderTest.*";
    //filter = "LineAgentTest.*";
    //filter = "LineListTest.*";
    //filter = "LoggerTest.*";
    //filter = "MatcherTest.*";
    //filter = "ParserTest.*";
    //filter = "PathTest.*";
    //filter = "SearchEngineTest.*";
    //filter = "StorageTest.*";
    //filter = "StringListTest.*";
    //filter = "StringToolTest.*";
    //filter = "TimeToolTest.*";
    //filter = "TraverserTest.*";
    //filter = "TextKnifeTest.*";
    std::string arg = "--gtest_filter=";
    arg += filter;
    char *args[] = { (char*) "dummy", const_cast<char*>(arg.c_str()), nullptr };
    googleTest(2, args);
  } else {
    textKnife(argc - 1, argv + 1);
  }
  return EXIT_SUCCESS;
}

