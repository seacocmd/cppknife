/*
 * File_test.cpp
 *
 *  Created on: 24.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../os/os.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(FileTest, fileSystemEntityBasics) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    FileSystemEntity entry(FT_FILE, "/etc/passwd");
    ASSERT_STREQ("/etc/passwd", entry.fullName().c_str());
    ASSERT_STREQ("/etc", entry.path().c_str());
    ASSERT_STREQ("passwd", entry.node().c_str());
    FileSystemEntity entry2(FT_FILE, "passwd", "/etc");
    ASSERT_STREQ("/etc/passwd", entry2.fullName().c_str());
    ASSERT_STREQ("/etc", entry2.path().c_str());
    ASSERT_STREQ("passwd", entry2.node().c_str());
    FileSystemEntity entry3(FT_UNDEF, "");
    entry3 = entry2;
    ASSERT_STREQ("/etc/passwd", entry3.fullName().c_str());
    ASSERT_STREQ("/etc", entry3.path().c_str());
    ASSERT_STREQ("passwd", entry3.node().c_str());
  } else {
    throw InternalError("missing FileTool::fileSystemEntityBasics test");
  }
}
TEST(FileTest, fileSystemEntityExists) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    FileSystemEntity entry(FT_FILE, "/etc/passwd");
    ASSERT_TRUE(entry.exists());
    FileSystemEntity entry2(FT_FILE, "default", "/etc");
    ASSERT_TRUE(entry.exists());

  } else {
    throw InternalError("missing FileTool::fileSystemEntityExists test");
  }
}
TEST(FileTest, fileSystemEntityCreated) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    FileSystemEntity entry(FT_FILE, "/etc/passwd");
    ASSERT_NE(UNDEF_TIME, entry.created());
    FileSystemEntity entry2(FT_FILE, "/does.not.exist/djfkldaj");
    ASSERT_EQ(UNDEF_TIME, entry2.created());
  } else {
    throw InternalError("missing FileTool::fileSystemEntityCreated test");
  }
}
TEST(FileTest, fileSystemEntityModified) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    FileSystemEntity entry(FT_DIRECTORY, "/etc/default");
    ASSERT_NE(UNDEF_TIME, entry.modified());
    FileSystemEntity entry2(FT_DIRECTORY, "/does.not.exist/djfkldaj");
    ASSERT_EQ(UNDEF_TIME, entry2.modified());
  } else {
    throw InternalError("missing FileTool::fileSystemEntityModified test");
  }
}
TEST(FileTest, fileBasics) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    File entry("/etc/group");
    ASSERT_STREQ("/etc/group", entry.fullName().c_str());
    ASSERT_STREQ("/etc", entry.path().c_str());
    ASSERT_STREQ("group", entry.node().c_str());
    File entry2("group", "/etc");
    ASSERT_STREQ("/etc/group", entry2.fullName().c_str());
    ASSERT_STREQ("/etc", entry2.path().c_str());
    ASSERT_STREQ("group", entry2.node().c_str());
    File entry3("");
    entry3 = entry2;
    ASSERT_STREQ("/etc/group", entry3.fullName().c_str());
    ASSERT_STREQ("/etc", entry3.path().c_str());
    ASSERT_STREQ("group", entry3.node().c_str());
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileTest, fileExists) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    File entry("/etc/passwd");
    ASSERT_TRUE(entry.size() != UNDEF_SIZE);
    File entry2("/does.not.exist/fjdkaldf");
    ASSERT_EQ(entry2.size(), UNDEF_SIZE);
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileTest, directoryBasics) {
  if (osInfo()._osType == LINUX) {
    Directory entry("/etc/default");
    Directory entry2(entry);
    ASSERT_STREQ("/etc/default", entry2.fullName().c_str());
    ASSERT_STREQ("/etc", entry2.path().c_str());
    ASSERT_STREQ("default", entry2.node().c_str());
    Directory entry3("");
    entry3 = entry2;
    ASSERT_STREQ("/etc/default", entry3.fullName().c_str());
    ASSERT_STREQ("/etc", entry3.path().c_str());
    ASSERT_STREQ("default", entry3.node().c_str());
  } else {
    throw InternalError("missing FileTool::fileExists test");
  }
}
TEST(FileTest, SymbolicLinksBasics) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    SymbolicLink entry("/etc/mtab");
    ASSERT_STREQ("/etc/mtab", entry.fullName().c_str());
    ASSERT_STREQ("/etc", entry.path().c_str());
    SymbolicLink entry2("mtab", "/etc");
    ASSERT_STREQ("/etc/mtab", entry2.fullName().c_str());
    ASSERT_STREQ("/etc", entry2.path().c_str());
    ASSERT_STREQ("mtab", entry2.node().c_str());
    SymbolicLink entry3("");
    entry3 = entry2;
    ASSERT_STREQ("/etc/mtab", entry3.fullName().c_str());
    ASSERT_STREQ("/etc", entry3.path().c_str());
    ASSERT_STREQ("mtab", entry3.node().c_str());
  } else {
    throw InternalError("missing FileTool::SymbolicLinksBasics test");
  }
}

TEST(FileTest, copyFile) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    auto source = "/etc/fstab";
    auto target = "/tmp/fstab.01";
    std::string errorMessage;
    ASSERT_TRUE(copyFile(source, target, &errorMessage));
    struct stat stateSource, stateTarget;
    ASSERT_EQ(0, ::stat(source, &stateSource));
    ASSERT_EQ(0, ::stat(target, &stateTarget));
    ASSERT_EQ(stateSource.st_mtime, stateTarget.st_mtime);
    ASSERT_EQ(stateSource.st_size, stateTarget.st_size);
  } else {
    throw InternalError("missing FileTool::SymbolicLinksBasics test");
  }
}

TEST(FileTest, copyFileError) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    auto source = "/etc/shadow";
    auto target = "/tmp/shadow.01";
    std::string errorMessage;
    ASSERT_FALSE(copyFile(source, target, &errorMessage));
    printf("expected error: %s\n", errorMessage.c_str());
    ASSERT_FALSE(errorMessage.empty());
  } else {
    throw InternalError("missing FileTool::SymbolicLinksBasics test");
  }
}

TEST(FileTest, uniqueFilename) {
  FEW_TESTS();
  if (osInfo()._osType == LINUX) {
    auto source = "/tmp/file.txt";
    auto source2 = "/tmp/file.01.txt";
    auto source3 = "/tmp/file.02.txt";
    ::unlink(source);
    ::unlink(source2);
    ::unlink(source3);
    ASSERT_STREQ(source, uniqueFilename(source, 2).c_str());
    writeText(source, "");
    ASSERT_STREQ(source2, uniqueFilename(source, 2).c_str());
    writeText(source2, "");
    ASSERT_STREQ(source3, uniqueFilename(source, 2).c_str());
    ASSERT_STREQ(source3, uniqueFilename(source2, 2).c_str());
  } else {
    throw InternalError("missing FileTool::SymbolicLinksBasics test");
  }
}

