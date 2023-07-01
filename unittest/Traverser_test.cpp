/*
 * Traverser_test.cpp
 *
 *  Created on: 22.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include <errno.h>
#include <fts.h>
using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

std::string createTree() {
  auto rc =
      buildFileTree(
          R"""(/tmp/unittest/traverser/file1.txt;332
/tmp/unittest/traverser/file3.txt;80
/tmp/unittest/traverser/dir1/file4.txt;85
/tmp/unittest/traverser/dir1/dir2/file3.txt;90
)""");
  return rc;
}
TEST(TraverserTest, basic) {
  FEW_TESTS();
  auto logger = buildMemoryLogger();
  DirEntryFilter filter;
  auto base = createTree();
  Traverser traverser(base.c_str(), &filter, nullptr, logger);
  int level;
  auto *entry = traverser.nextFile(level);
  ASSERT_TRUE(entry != nullptr);
  ASSERT_STREQ(entry->fullName(), "/tmp/unittest/traverser/file1.txt");
  entry = traverser.nextFile(level);
  ASSERT_TRUE(entry != nullptr);
  ASSERT_STREQ(entry->fullName(), "/tmp/unittest/traverser/file3.txt");
  delete logger;
}
TEST(TraverserTest, labor) {
  FEW_TESTS();
  auto base = createTree();
  FTS *fts;
  char *argv[] = { (char*) base.c_str(), nullptr };
  if ((fts = fts_open(argv, FTS_COMFOLLOW | FTS_PHYSICAL, nullptr))
      != nullptr) {
    if (errno != 0) {
      printf("+++ errno: %d\n", errno);
    } else {
      FTSENT *p;
      while ((p = fts_read(fts)) != nullptr) {
        //printf ("= %s %d\n", p->fts_path, p->fts_info);
        switch (p->fts_info) {
        case FTS_D:
          printf("d %s\n", p->fts_path);
          if (endsWith(p->fts_path, -1, "dir3")) {
            fts_set(fts, p, FTS_SKIP);
          }
          break;
        case FTS_DP:
          printf("d %s\n", p->fts_path);
          if (false && endsWith(p->fts_path, -1, "dir3")) {
            fts_set(fts, p, FTS_SKIP);
          }
          break;
        case FTS_F:
          printf("f %s\n", p->fts_path);
          break;
        case FTS_SL:
        case FTS_SLNONE:
          printf("l %s\n", p->fts_path);
          break;
        default:
          printf("? %s %d\n", p->fts_path, p->fts_info);
          break;
        }
      }
    }
    fts_close(fts);
  }
}

