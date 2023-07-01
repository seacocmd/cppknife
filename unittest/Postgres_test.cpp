/*
 * Postgres_test.cpp
 *
 *  Created on: 12.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#if ! defined IGNORE_POSTGRES

#include "google_test.hpp"
#include "../db/db.hpp"
static bool fewTests() {
  return false;
}
#define FEW_TESTS if (fewTests()) return

using namespace cppknife;

TEST(PostgresTest, execute) {
  FEW_TESTS;
  auto logger = buildMemoryLogger(100, LV_FINE);
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  ASSERT_TRUE(sql.execute("DROP TABLE IF EXISTS persons;"));
  ASSERT_TRUE(
      sql.execute(
          R"""(CREATE TABLE IF NOT EXISTS persons (
id SERIAL NOT NULL,
name VARCHAR(64),
prio INT DEFAULT 0,
PRIMARY KEY(id)
);)"""));
  ASSERT_TRUE(
      sql.execute("INSERT INTO persons (name, prio) VALUES ('adam', 1);"));
  delete logger;
}
TEST(PostgresTest, query) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  Result result(sql);
  ASSERT_TRUE(sql.query("SELECT count(*) from persons;", result));
  ASSERT_EQ(1, result._rows.size());
  auto value = result._rows[0]->asString(0);
  ASSERT_NE(nullptr, value);
  ASSERT_STREQ("1", value);
  delete logger;
}

TEST(PostgresTest, queryPrepared) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  Result result(sql);
  const char *params[] = {"1", nullptr};
  ASSERT_TRUE(
      sql.query("SELECT count(*) from persons where id>$1;", result, params,
          1));
  ASSERT_EQ(1, result._rows.size());
  auto value = result._rows[0]->asString(0);
  ASSERT_NE(nullptr, value);
  ASSERT_STREQ("0", value);
  delete logger;
}

TEST(PostgresTest, allTables) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  auto tables = sql.allTables();
  ASSERT_TRUE(tables.size() > 0);
  delete logger;
}
TEST(PostgresTest, replacePositionalArguments) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  const char *args[] = {"'", "2", "131", "4", "5", "6", "7", "8", "9", "X",
    nullptr};
  ASSERT_STREQ("select * from a where ''''='a''$2' or 131 and 'X'!='';",
      sql.replacePositionalArguments(
          "select * from a where $1='a''$2' or $3 and $10!='';", '$', args).c_str());
  delete logger;
}
TEST(PostgresTest, lastInsertedId) {
  //FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  sql.execute("INSERT INTO persons (name, prio) VALUES ('berta', 2);");
  auto id = sql.lastInsertedId();
  ASSERT_TRUE(id > 1);
  delete logger;
}
TEST(PostgresTest, affectedRows) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  sql.execute("UPDATE persons set prio=10 where name='adam';");
  ASSERT_EQ(1, sql._affectedRows);
  delete logger;
}
TEST(PostgresTest, affectedRowsDelete) {
  FEW_TESTS;
  auto logger = buildMemoryLogger();
  Postgres sql("pgtest", "test", "test", *logger);
  ASSERT_TRUE(sql.connect());
  sql.execute("DELETE FROM persons where name='berta';");
  ASSERT_EQ(1, sql._affectedRows);
  delete logger;
}

#endif /* ! defined IGNORE_POSTGRES */
