/*
 * MySql.hpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef DB_MYSQL_HPP_
#define DB_MYSQL_HPP_
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

namespace cppknife {

/**
 * @brief Stores the meta data of a database table.
 *
 * Meta data are infos about the structure: columns with datatypes...
 */
class TableMetaMysql: public TableMeta {
  std::string _tableName;
public:
  TableMetaMysql(const char *table, SqlDriver &driver);
  virtual ~TableMetaMysql();
protected:
  void findOptions(const std::string &text, ColumnMeta &info,
      const std::regex &reTypeDefinition, const std::regex &reSizeDefinition);
public:
  virtual void initialize();
};

/**
 * @brief  Implementation of the interface <em>SqlDriver</em> for MySql and Mariadb.
 */
class MySql: public SqlDriver {
protected:
  std::string _host;
  int _port;
  std::string _database;
  std::string _user;
  std::string _password;
  sql::Driver *_rawDriver;
  sql::Connection *_connection;
  sql::Statement *_statement;
  MetaResult *_metaResult;
public:
  bool _buildMetaResult;
public:
  MySql(const char *database, const char *user, const char *password,
      Logger &logger, const char *host = "localhost", int port = 3306);
  virtual ~MySql();
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

#endif /* DB_MYSQL_HPP_ */
