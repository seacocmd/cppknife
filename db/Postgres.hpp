/*
 * Postgres.hpp
 *
 *  Created on: 12.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef DB_POSTGRES_HPP_
#define DB_POSTGRES_HPP_

/// Installation packages: apt install libpqxx-6.4 or libpqxx-dev
#include <pqxx/pqxx>
namespace cppknife {

/**
 Manages a MySQL or Mariadb database.
 */
class Postgres: public SqlDriver {
protected:
  std::string _host;
  int _port;
  std::string _database;
  std::string _user;
  std::string _password;
  pqxx::connection *_connection;
public:
  Postgres(const char *database, const char *user, const char *password,
      Logger &logger, const char *host = "localhost", int port = 5432);
  virtual ~Postgres();
public:
  virtual std::vector<std::string> allDbs(bool withSystemDatabases = false);
  virtual std::vector<std::string> allTables();
  void close();
  bool connect();
  virtual bool createDbAndUser(const char *dbName, const char *user = nullptr,
      const char *password = nullptr, bool isAdministrator = false,
      bool readOnly = false, const char *group = nullptr);
  virtual bool execute(const char *sql, const char **arguments = nullptr);
  virtual int64_t lastInsertedId();
  virtual bool loadDb(const char *database, const char *filename,
      Configuration &configuration);
  virtual bool query(const char *sql, Result &result, const char **arguments =
      nullptr, int colCount = -1);
  virtual bool storeDb(const char *database, const char *filename,
      Configuration &configuration);
  virtual bool summaryOf(const char *table, TableSummary &result);
};

} /* namespace cppknife */

#endif /* DB_POSTGRES_HPP_ */
