/*
 * Path_test.cpp
 *
 *  Created on: 16.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

using namespace cppknife;
TEST(PathTest, pathInfoJoin) {
  PathInfo info;
  splitPath("/abc/def/jonny.test", info);
  ASSERT_STREQ(info.join().c_str(), "/abc/def/jonny.test");
}

TEST(PathTest, joinPath) {
  if (osInfo()._osType == LINUX) {
    ASSERT_STREQ(joinPath(nullptr, "name", "extension").c_str(),
        "name.extension");
    ASSERT_STREQ(joinPath(nullptr, "name", ".txt").c_str(), "name.txt");
    ASSERT_STREQ(joinPath("/tmp/", "name", ".txt").c_str(), "/tmp/name.txt");
    ASSERT_STREQ(joinPath("/tmp", "name").c_str(), "/tmp/name");
    ASSERT_STREQ(joinPath("/tmp", "name", ".ext", "dir1").c_str(),
        "/tmp/dir1/name.ext");
    ASSERT_STREQ(joinPath("/tmp", "name", ".ext", "/dir1/dir2/").c_str(),
        "/tmp/dir1/dir2/name.ext");
  }
}
TEST(PathTest, splitPath) {
  if (osInfo()._osType == LINUX) {
    PathInfo info;
    splitPath("/abc/def/jonny.test", info);
    ASSERT_STREQ(info._path.c_str(), "/abc/def/");
    ASSERT_STREQ(info._name.c_str(), "jonny");
    ASSERT_STREQ(info._extension.c_str(), ".test");
    splitPath("abc/def/.git", info);
    ASSERT_STREQ(info._path.c_str(), "abc/def/");
    ASSERT_STREQ(info._name.c_str(), ".git");
    ASSERT_STREQ(info._extension.c_str(), "");
    splitPath("happy.to.live.git", info);
    ASSERT_STREQ(info._path.c_str(), "");
    ASSERT_STREQ(info._name.c_str(), "happy.to.live");
    ASSERT_STREQ(info._extension.c_str(), ".git");
    splitPath(".configuration", info);
    ASSERT_STREQ(info._path.c_str(), "");
    ASSERT_STREQ(info._name.c_str(), ".configuration");
    ASSERT_STREQ(info._extension.c_str(), "");
    splitPath("configuration", info);
    ASSERT_STREQ(info._path.c_str(), "");
    ASSERT_STREQ(info._name.c_str(), "configuration");
    ASSERT_STREQ(info._extension.c_str(), "");
  } else {
    ASSERT_TRUE(false);
  }
}
TEST(PathTest, separators) {
  ASSERT_EQ(separatorChar(), osInfo()._separatorChar);
  ASSERT_EQ(separatorString(), osInfo()._separatorString);
}
TEST(PathTest, temporaryFile) {
  if (osInfo()._osType == LINUX) {
    ASSERT_EQ(temporaryFile("hi.txt", "dir1/dir2"), "/tmp/dir1/dir2/hi.txt");
    ASSERT_EQ(temporaryFile("hi.txt", "dir1"), "/tmp/dir1/hi.txt");
    ASSERT_EQ(temporaryFile("hi.txt"), "/tmp/hi.txt");
    ASSERT_EQ(separatorString(), osInfo()._separatorString);
  } else {
    ASSERT_TRUE(false);
  }
}
TEST(PathTest, dirname) {
  if (osInfo()._osType == LINUX) {
    ASSERT_STREQ("/dir", dirname("/dir/abc.txt").c_str());
    ASSERT_STREQ("/dir/abc/", dirname("/dir/abc/def.txt", true).c_str());
    ASSERT_STREQ("", dirname("abc.txt").c_str());
    ASSERT_STREQ("", dirname("abc.txt", true).c_str());
  } else {
    ASSERT_TRUE(false);
  }
}

TEST(PathTest, replaceExtension) {
  if (osInfo()._osType == LINUX) {
    ASSERT_STREQ("data.txt", replaceExtension("data.doc", ".txt").c_str());
    ASSERT_STREQ("data.txt", replaceExtension("data.doc", "txt").c_str());
    ASSERT_STREQ("data.txt", replaceExtension("data", "txt").c_str());
    ASSERT_STREQ("data.txt", replaceExtension("data", ".txt").c_str());
    ASSERT_STREQ("/dir.2/data.txt",
        replaceExtension("/dir.2/data.doc", ".txt").c_str());
    ASSERT_STREQ("/dir.2/data.txt",
        replaceExtension("/dir.2/data.doc", "txt").c_str());
    // dot at first place is not an extension dot:
    ASSERT_STREQ("/dir.2/.config.conf",
        replaceExtension("/dir.2/.config", "conf").c_str());
    ASSERT_STREQ("/dir.2/.config.conf",
        replaceExtension("/dir.2/.config", ".conf").c_str());
    // slash behind the dot:
    ASSERT_STREQ("/dir.2/data.txt",
        replaceExtension("/dir.2/data", ".txt").c_str());
    ASSERT_STREQ("/dir.2/data.txt",
        replaceExtension("/dir.2/data", "txt").c_str());
    // remove extension:
    ASSERT_STREQ("data", replaceExtension("data.txt", nullptr).c_str());
    ASSERT_STREQ("/dir.2/data",
        replaceExtension("/dir.2/data.txt", nullptr).c_str());
    ASSERT_STREQ(".config", replaceExtension(".config", nullptr).c_str());
  } else {
    ASSERT_TRUE(false);
  }
}

