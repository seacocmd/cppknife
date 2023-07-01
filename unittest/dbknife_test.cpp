/*
 * dbknife_test.cpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../tools/dbknife.hpp"

#include "google_test.hpp"
namespace cppknife {

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(DbKnifeTest, veil) {
  FEW_TESTS();
  auto fnSource = temporaryFile("veil.txt", "unittest", true);
  auto fnTarget = temporaryFile("veil.data", "unittest", true);
  writeText(fnSource.c_str(),
      R"""(# Test configuration
db.name = abc
db.timeout = 30
session.timeout = 20
)""");
  const char *argv[] = { "-l7", "veil", "--user=unittest", "unittest",
      fnSource.c_str(), fnTarget.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  dbknifeUnderTest = true;
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  auto lines2 = readAsList(fnTarget.c_str(), logger);
  ASSERT_EQ(5, lines2.size());
  ASSERT_EQ(20 + 4, lines2[0].size());
  ASSERT_EQ(0, lines2[4].size());
  delete logger;
}

TEST(DbKnifeTest, unveil) {
  FEW_TESTS();
  auto fnSource = temporaryFile("veil.txt", "unittest", true);
  auto fnTarget = temporaryFile("veil.data", "unittest", true);
  auto fnData = temporaryFile("veil2.txt", "unittest", true);
  auto text =
      R"""(# A configuration file
      abc.def = 123
      xxx.zzz = "abc")""";
  writeText(fnSource.c_str(), text);
  const char *argv[] = { "-l7", "veil", "--user=unittest", "unittest",
      fnSource.c_str(), fnTarget.c_str() };
  auto logger = buildMemoryLogger(100, LV_FINE);
  dbknifeUnderTest = true;
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  const char *argv2[] = { "-l7", "unveil", "--user=unittest", "unittest",
      fnTarget.c_str(), fnData.c_str() };
  dbknife(sizeof argv2 / sizeof argv2[0], const_cast<char**>(argv2), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  auto lines2 = readAsString(fnData.c_str(), logger);
  ASSERT_STREQ(text, lines2.c_str());
  delete logger;
}

TEST(DbKnifeTest, unveil2) {
  //FEW_TESTS();
#if defined EVER
  auto fnSource = "/etc/cppknife/db.conf";
  auto fnData = temporaryFile("veil2.txt", "unittest", true);
  const char *argv[] = { "unveil", "--user=seacocmd", "db", fnSource, "" };
  auto logger = buildMemoryLogger(100, LV_FINE);
  dbknifeUnderTest = false;
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  auto lines = appender->lines();
  auto lines2 = readAsString(fnData.c_str(), logger);
  delete logger;
#endif
}

TEST(DbKnifeTest, mailer) {
  FEW_TESTS();
  auto path = temporaryFile("mailer", "unittest", true);
  ensureDirectory(path.c_str());
  auto fnSource = joinPath(path.c_str(), "1234321");
  auto fnStop = joinPath(path.c_str(), "stop!");
  auto data =
      R"""(seacocmd@gmail.com
options:
Mail from unittest mailer
Line1
Line2
)""";
  writeText(fnSource.c_str(), data);
  writeText(fnStop.c_str(), "");
  const char *argv[] = { "-l7", "mailer", "--tasks", path.c_str(),
      "--interval=1" };
  auto logger = buildMemoryLogger(100, LV_FINE);
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  if (appender != nullptr) {
    auto lines = appender->lines();
    ASSERT_EQ(1, lines.size());
  }
  delete logger;
}

TEST(DbKnifeTest, licenseUnlicense) {
  FEW_TESTS();
  extern bool dbknifeUnderTest;
  dbknifeUnderTest = true;
  auto logger = buildMemoryLogger(100, LV_FINE);
  const char *argv[] = { "-l7", "license", "--host", "*", "-3", "!",
      "--additional-secret", "db.client=Never", "--additional-secret",
      "db.server=Always", "-auser.unittest=x", "-aapplication.unittest=y" };
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  ASSERT_NE(appender, nullptr);
  auto lines = appender->lines();
  if (dbknifeUnderTest) {
    ASSERT_EQ(2, lines.size());
  }
  auto fn = temporaryFile("example.license");
  ASSERT_TRUE(fileExists(fn.c_str()));
  const char *argv2[] = { "-l7", "unlicense", fn.c_str() };
  appender->clear();
  dbknife(sizeof argv2 / sizeof argv2[0], const_cast<char**>(argv2), logger);
  lines = appender->lines();
  ASSERT_EQ(10, lines.size());
  ASSERT_STREQ(lines[0].c_str(), "# veil engine configuration:");
  ASSERT_STREQ("db.server=Always", lines[5].c_str());
  delete logger;
}
TEST(DbKnifeTest, listDbs) {
  FEW_TESTS();
  extern bool dbknifeUnderTest;
  dbknifeUnderTest = true;
  auto logger = buildMemoryLogger(100, LV_FINE);
  const char *argv[] = { "-l7", "list-dbs", "--system" };
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  ASSERT_NE(appender, nullptr);
  auto lines = appender->lines();
  ASSERT_TRUE(lines.size() > 3);
}
TEST(DbKnifeTest, listDbsPattern) {
  FEW_TESTS();
  extern bool dbknifeUnderTest;
  dbknifeUnderTest = true;
  auto logger = buildMemoryLogger(100, LV_FINE);
  const char *argv[] = { "-l7", "list-dbs", "--system", "--pattern=*mysql*" };
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  ASSERT_NE(appender, nullptr);
  auto lines = appender->lines();
  ASSERT_STREQ(lines[0].c_str(), "mysql");
}
TEST(DbKnifeTest, createDb) {
  FEW_TESTS();
  extern bool dbknifeUnderTest;
  dbknifeUnderTest = true;
  auto logger = buildMemoryLogger(100, LV_FINE);
  const char *argv[] = { "-l7", "create-db", "--user=dbaunittest:abc",
      "--administrator", "dbunittest" };
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  ASSERT_NE(appender, nullptr);
  auto lines = appender->lines();
  ASSERT_TRUE(lines.size() >= 1);
  ASSERT_STREQ("= db created: dbunittest", lines[0].c_str());
  ASSERT_STREQ(
      "grant all on dbunittest.* to 'dbaunittest'@'localhost' identified by 'abc' with grant option;",
      lines[1].c_str());
}
TEST(DbKnifeTest, createDbReadOnly) {
  FEW_TESTS();
  extern bool dbknifeUnderTest;
  dbknifeUnderTest = true;
  auto logger = buildMemoryLogger(100, LV_FINE);
  const char *argv[] = { "-l7", "create-db", "--user=dbrounittest:abc",
      "--read-only" };
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  ASSERT_NE(appender, nullptr);
  auto lines = appender->lines();
  ASSERT_TRUE(lines.size() >= 1);
  ASSERT_STREQ(
      "grant SELECT,SHOW VIEW,TRIGGER,PROCESS,LOCK TABLES on *.* to 'dbrounittest'@'localhost' identified by 'abc';",
      lines[1].c_str());
}
TEST(DbKnifeTest, info) {
  FEW_TESTS();
  extern bool dbknifeUnderTest;
  dbknifeUnderTest = true;
  auto logger = buildMemoryLogger(100, LV_FINE);
  const char *argv[] = { "-l7", "info", "test.person*" };
  dbknife(sizeof argv / sizeof argv[0], const_cast<char**>(argv), logger);
  auto appender = dynamic_cast<MemoryAppender*>(logger->findAppender("memory"));
  ASSERT_NE(appender, nullptr);
  auto lines = appender->lines();
  ASSERT_TRUE(lines.size() >= 1);
  auto line1 = lines[0];
  ASSERT_STREQ("= database test", line1.c_str());
  ASSERT_TRUE(startsWith(lines[1].c_str(), lines[1].size(), "person"));
}
} /* namespace cppknife */

