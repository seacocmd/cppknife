/*
 * cppknife.cpp
 *
 *  Created on: 17.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
/**
 * @mainpage
 * <h1>Objectives</h1>
 * The package <strong>cppknife</strong> offers classes for different scopes:
 * <ul><li><strong>basic</strong>: Most often used classes: Logger InternalError StringTool TimeTool ...</li>
 * <li><strong>core</strong>: Often used classes for general usage: ByteStorage KissRandom Storage.</li>
 * <li><strong>db</strong>: Defines an abstract class SqlDriver with the implementations MySql and Postgres.</li>
 * <li><strong>geo</strong>: Defines some classes for geometric and geographical things: Shape Rectangle Point2D GpsTranslator... </li>
 * <li><strong>os</strong>: Defines some classes for operating system things: File FileTool OsException Process Traverser...</li>
 * <li><strong>text</strong>: Defines some classes for text/text file handling: Configuration CsvFile JsonReader Matcher Parser SearchEngine StringList...</li>
 * <li><strong>text</strong>: Here are the applications and application related support files/classes: ArgumentParser dbknife fileknife textknife sesknife ...</li>
 * </ul>
 */
#include <stdio.h>
#include "basics.hpp"
using cppknife;

extern int
googleTest (int argc, char **argv);

int
main (int argc, char **argv)
{
  if (argc > 1 && strcmp (argv[1], "--test") == 0)
    {
      //char* args[] = { (char*)"dummy", (char*)"----gtest_filter=*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=ByteStorageTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=StorageTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=StringListTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=PathTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=FileToolTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=FileTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=LoggerTest.*", nullptr };
      //char* args[] = { (char*)"dummy", (char*)"--gtest_filter=BaseRandomTest.*", nullptr };
      char* args[] = { (char*)"dummy", (char*)"--gtest_filter=TraverserTest.*", nullptr };
      googleTest (2, args);
    }
  else
    {

    }
  return EXIT_SUCCESS;
}
