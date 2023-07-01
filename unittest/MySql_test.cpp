/*
 * MySql_test.cpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#if ! defined IGNORE_MYSQL
#include "google_test.hpp"
#include "../db/db.hpp"

using namespace cppknife;
static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

TEST(MySqlTest, execute) {
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  ASSERT_TRUE(sql.execute("DROP TABLE IF EXISTS persons;"));
  ASSERT_TRUE(
      sql.execute(
          R"""(CREATE TABLE IF NOT EXISTS persons (
id INT AUTO_INCREMENT NOT NULL,
name VARCHAR(64),
prio INT DEFAULT 0,
PRIMARY KEY(id)
);)"""));
  ASSERT_TRUE(
      sql.execute("INSERT INTO persons (name, prio) VALUES ('adam', 1);"));
  delete logger;
}
TEST(MySqlTest, query) {
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  Result result(sql);
  ASSERT_TRUE(sql.query("SELECT count(*) from persons;", result));
  ASSERT_EQ(1, result._rows.size());
  auto value = result._rows[0]->asString(0);
  ASSERT_NE(nullptr, value);
  ASSERT_STREQ("1", value);
  delete logger;
}

TEST(MySqlTest, allTables) {
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  auto tables = sql.allTables();
  ASSERT_TRUE(tables.size() > 0);
}

TEST(MySqlTest, replacePositionalArguments) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  const char *args[] = {"'", "2", "131", "4", "5", "6", "7", "8", "9", "X",
    nullptr};
  ASSERT_STREQ("select * from a where ''''='a''$2' or 131 and 'X'!='';",
      sql.replacePositionalArguments(
          "select * from a where $1='a''$2' or $3 and $10!='';", '$', args).c_str());
  delete logger;
}
TEST(MySqlTest, lastInsertedId) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  sql.execute("INSERT INTO persons (name, prio) VALUES ('berta', 2);");
  auto id = sql.lastInsertedId();
  ASSERT_TRUE(id > 1);
  delete logger;
}
TEST(MySqlTest, affectedRows) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  sql.execute("UPDATE persons set prio=10 where name='adam';");
  ASSERT_EQ(1, sql._affectedRows);
  delete logger;
}
TEST(MySqlTest, affectedRowsDelete) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  MySql sql("test", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  sql.execute("DELETE FROM persons where name='berta';");
  ASSERT_EQ(1, sql._affectedRows);
  delete logger;
}
#endif /* ! defined IGNORE_MYSQL */

