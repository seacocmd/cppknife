/*
 * SqlDriver.hpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef DB_SQLDRIVER_HPP_
#define DB_SQLDRIVER_HPP_

namespace cppknife {
class Result;
enum ColumnType {
  COLTYPE_UNDEF,
  COLTYPE_INT,
  COLTYPE_FLOAT,
  COLTYPE_VARCHAR,
  COLTYPE_CHAR,
  COLTYPE_TEXT,
  COLTYPE_TIMESTAMP,
  COLTYPE_DATE,
  COLTYPE_TIME
};
class SqlDriver;
/**
 * Stores the meta data of a database column.
 */
class ColumnMeta {
public:
  ColumnType _type;
  int _size;
  int _precision;
  std::string _name;
};
/**
 * Base class of manager classes that store the meta data of a database table.
 */
class TableMeta {
protected:
  SqlDriver &_driver;
  std::map<std::string, ColumnMeta*> _columns;
public:
  TableMeta(SqlDriver &driver);
  virtual ~TableMeta();
public:
  virtual void clear();
  virtual void initialize() = 0;
};
class TableSummary {
public:
  int _rowCount;
  int _colCount;
  std::vector<std::string> _columns;
public:
  TableSummary();
public:
  void clear();
};
/**
 * Stores the meta data of a result set of a database query.
 */
class MetaResult {
public:
  std::vector<std::string> _columnNames;
  Result &_parent;
public:
  MetaResult(Result &parent);
  virtual ~MetaResult();
public:
  int indexOf(const char *column) const;
};
/**
 * Stores one row of a database query.
 */
class RowResult {
public:
  std::vector<std::string> _values;
  Result &_parent;
public:
  RowResult(Result &parent);
  virtual ~RowResult();
public:
  const char* asString(int index) const;
  const char* asString(const char *column) const;
  int asInt(int index) const;
  int asInt(const char *column) const;
};

/**
 * Stores the result of a database query.
 */
class Result {
public:
  SqlDriver &_driver;
  MetaResult *_meta;
  std::vector<RowResult*> _rows;
  int _colCount;
  void *_nativeMeta;
  void *_nativeResult;
public:
  Result(SqlDriver &driver);
  virtual ~Result();
public:
  void clear();
  inline int colCount() const {
    return _colCount;
  }
  inline int indexOf(const char *column) const {
    return _meta->indexOf(column);
  }
};
class LoginData {
public:
  const char *_driver;
  const char *_db;
  const char *_user;
  const char *_code;
  const char *_host;
  int _port;
public:
  LoginData(const char *driver, const char *database = nullptr);
  LoginData(const char *driver, Configuration &configuration,
      const char *database = nullptr, bool readOnly = false);
public:
  bool fromConfiguration(Configuration &configuration, bool readOnly = false);
};
/**
 * Base class of a database connection manager.
 * That allows to write programs that can connect to different database management systems.
 */
class SqlDriver {
public:
  static std::string _default_driver;
  int _affectedRows;
protected:
  std::string _databaseName;
  Logger &_logger;
  int64_t _lastInsertedId;
public:
  SqlDriver(const char *tableName, Logger &logger);
  virtual ~SqlDriver();
public:
  /**
   * Returns the databases.
   * @param withSystemDatabases true: system databases will be part of the result
   * @return The database names.
   */
  virtual std::vector<std::string> allDbs(bool withSystemDatabases = false) = 0;
  /**
   * Returns the table names of the current database.
   */
  virtual std::vector<std::string> allTables() = 0;
  /**
   * Creates a database and/or an user.
   * @param dbName <em>nullptr</em> or the database name.
   * @param user <em>nullptr</em> or the name of the user to create.
   * @param password The password of the user to create.
   * @param isAdministrator <em>true</em>: the rights of the user will be extended.
   * @param readOnly <em>true</em>: the rights are restricted to read only access.
   * @param group <em>nullptr</em> or the group the new user is belonging to.
   */
  virtual bool createDbAndUser(const char *dbName, const char *user = nullptr,
      const char *password = nullptr, bool isAdministrator = false,
      bool readOnly = false, const char *group = nullptr) = 0;
  /**
   * Returns the name of the current database.
   */
  const std::string databaseName() const {
    return _databaseName;
  }
  /**
   * Executes a SQL statement without results, e.g. creation of a table.
   * @param sql The SQL statement, e.g. "delete from users;";
   * @param arguments <em>nullptr</em> or the arguments for prepared SQL statements.
   * @return <em>true</em>: success. Otherwise: an error has occurred.
   */
  virtual bool execute(const char *sql, const char **arguments = nullptr) = 0;
  /**
   * Returns the primary key of the last inserted id if the table have the AUTO_INCREMENT attribute.
   */
  virtual int64_t lastInsertedId() {
    return _lastInsertedId;
  }
  /**
   * Loads the database content from a SQL text file (or gzipped).
   * @param database The name of the database.
   * @param filename The name of the file to read
   * @param configuration The configuratin is read from that.
   * @return true: success
   */
  virtual bool loadDb(const char *database, const char *filename,
      Configuration &configuration) = 0;
  /**
   * Returns the current logger.
   */
  inline Logger& logger() {
    return _logger;
  }
  /**
   * Performs a database query: that is a SQL statement with a result.
   * @param sql The SQL statement, e.g. "delete from users;";
   * @param result OUT: the result is stored here.
   * @param arguments <em>nullptr</em> or the arguments for prepared SQL statements.
   * @param colCount -1: column count is retrieved automatically. Otherwise: the number of columns of the query.
   * @return <em>true</em>: success. Otherwise: an error has occurred.
   */
  virtual bool query(const char *sql, Result &result, const char **arguments =
      nullptr, int colCount = -1) = 0;
  /**
   * Replaces the positional parameters with the given arguments in a SQL statement.
   * Example: statement: "select * from t where id>?;" parameterChar: '?' parameters[0]: 339
   * Result: select * from t where id>'339';
   * @param sql The SQL statement with positional arguments.
   * @param parameterChar The character starting (or being) the positional argument:
   *  '?' (mysql) or '$' (followed by the positional number) (postgres)
   * @param parameters The array with the values of the parameters.
   * @return The SQL code with replaced values of the positional arguments.
   */
  std::string replacePositionalArguments(const char *sql, char parameterChar,
      const char **parameters);
  /**
   * Stores the database content in a SQL text file (or gzipped).
   * @param database The name of the database.
   * @param filename The name of the file to create.
   * @param configuration Contains the configuration data.
   * @return true: success
   */
  virtual bool storeDb(const char *database, const char *filename,
      Configuration &configuration) = 0;
  /**
   * Returns an info about a given table.
   * @param table The table name.
   * @param result OUT: the result is stored here.
   * @return <em>true</em>: success. Otherwise: the table is unknown.
   */
  virtual bool summaryOf(const char *table, TableSummary &result) = 0;
};

} /* namespace cppknife */

#endif /* DB_SQLDRIVER_HPP_ */
