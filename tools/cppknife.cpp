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
 * <ul><li>The package polygeo offers classes for processing geometry problems with points, rectangles and polygons.
 * </li>
 * <li>There is the possibility to draw geometric objects as SVG images in a HTML file.</li>
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
