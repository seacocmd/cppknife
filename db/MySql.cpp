/*
 * MySql.cpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "db.hpp"

namespace cppknife {

TableMetaMysql::TableMetaMysql(const char *table, SqlDriver &driver) :
    TableMeta(driver), _tableName(table) {
}
TableMetaMysql::~TableMetaMysql() {
}

void TableMetaMysql::findOptions(const std::string &text, ColumnMeta &info,
    const std::regex &reTypeDefinition, const std::regex &reSizeDefinition) {
  std::smatch matches;
  if (std::regex_match(text, matches, reTypeDefinition)) {
    auto value = matches[0];
    if (value == "int") {
      info._type = COLTYPE_INT;
    } else if (value == "varchar") {
      info._type = COLTYPE_VARCHAR;
    } else if (value == "char") {
      info._type = COLTYPE_CHAR;
    } else if (value == "decimal") {
      info._type = COLTYPE_FLOAT;
    } else if (value == "text") {
      info._type = COLTYPE_TEXT;
    } else if (value == "timestamp") {
      info._type = COLTYPE_TIMESTAMP;
    } else if (value == "time") {
      info._type = COLTYPE_TIME;
    } else if (value == "date") {
      info._type = COLTYPE_DATE;
    } else {
      _driver.logger().say(LV_ERROR,
          formatCString("unknown data type: %s", value.str()));
    }
  }

}
void TableMetaMysql::initialize() {
  Result result(_driver);
  auto driver = dynamic_cast<MySql&>(_driver);
  driver.query(
      formatCString("SHOW CREATE TABLE %s;", _tableName.c_str()).c_str(),
      result);
  auto lines = splitCString(result._rows[0]->asString(0), "\n");
  std::regex reColumnDefinition("^`(\\w+)` (\\S+)");
  std::regex reTypeDefinition("^(\\w+)");
  std::regex reSizeDefinition("\\((\\d+)\\)");
  for (auto line : lines) {
    std::smatch matches;
    if (std::regex_match(line, matches, reColumnDefinition)) {
      auto name = matches[0];
      auto type = matches[1];
      // Will be deleted in clear():
      auto info = new ColumnMeta();
      _columns[name] = info;
      info->_name = name;
      findOptions(type, *info, reTypeDefinition, reSizeDefinition);
    }
  }
}

MySql::MySql(const char *database, const char *user, const char *password,
    Logger &logger, const char *host, int port) :
    SqlDriver(database, logger), _host(host), _port(port), _database(database), _user(
        user), _password(password), _rawDriver(nullptr), _connection(nullptr), _statement(
        nullptr), _metaResult(nullptr), _buildMetaResult(false) {
}

MySql::~MySql() {
  close();
}

std::vector<std::string> MySql::allDbs(bool withSystemTables) {
  std::vector<std::string> rc;
  Result result(*this);
  query("SHOW databases;", result, nullptr, 1);
  rc.reserve(result._rows.size());
  for (auto row : result._rows) {
    auto table = row->asString(0);
    if (withSystemTables
        || (strcmp(table, "mysql") != 0
            && strcmp(table, "information_schema") != 0
            && strcmp(table, "performance_schema") != 0)) {
      rc.push_back(table);
    }
  }
  return rc;
}
std::vector<std::string> MySql::allTables() {
  std::vector<std::string> rc;
  Result result(*this);
  query("SHOW tables;", result, nullptr, 1);
  rc.reserve(result._rows.size());
  for (auto row : result._rows) {
    rc.push_back(row->asString(0));
  }
  return rc;
}
void MySql::close() {
  if (_statement != nullptr) {
    _statement->close();
    delete _statement;
    _statement = nullptr;
  }
  if (_connection != nullptr) {
    _connection->close();
    delete _connection;
    _connection = nullptr;
  }
  delete _metaResult;
  _metaResult = nullptr;
// _rawDriver is global. Do not delete
}

bool MySql::connect() {
  bool rc = false;
  try {
    /* Create a connection */
    _rawDriver = get_driver_instance();
    auto connectString = formatCString("tcp://%s:%d", _host.c_str(), _port);
    _connection = _rawDriver->connect(connectString.c_str(), _user.c_str(),
        _password.c_str());
    if (_connection == nullptr) {
      _logger.say(LV_ERROR,
          formatCString("cannot connect to %s", connectString.c_str()));
    } else {
      /* Connect to the MySQL test database */
      _connection->setSchema(_database.c_str());
      _statement = _connection->createStatement();
      if (_statement == nullptr) {
        _logger.say(LV_ERROR,
            formatCString("cannot create STATEMENT on %s",
                connectString.c_str()));
      } else {
        rc = true;
      }
    }
  } catch (sql::SQLException &e) {
    _logger.say(LV_ERROR,
        formatCString("Connection failed: %s (%d) %s", e.what(),
            e.getErrorCode(), e.getSQLState()));
  }
  return rc;
}

bool MySql::createDbAndUser(const char *dbName, const char *user,
    const char *password, bool isAdministrator, bool readOnly,
    const char *group) {
  bool rc = true;
  if (dbName != nullptr) {
    execute(formatCString("CREATE DATABASE IF NOT EXISTS %s;", dbName).c_str());
    _logger.say(LV_DETAIL, formatCString("= db created: %s", dbName).c_str());
  }
  if (user != nullptr) {
    const char *admin = isAdministrator ? " with grant option" : "";
    //grant all on *.* to 'debian-sys-maint'@'localhost' identified by "6QLw8BSKFwzDnoUT" with grant option;
    const char *rights =
        !readOnly ? "all" : "SELECT,SHOW VIEW,TRIGGER,PROCESS,LOCK TABLES";
    auto sql = formatCString(
        "grant %s on %s.* to '%s'@'%s' identified by '%s'%s;", rights,
        dbName == nullptr ? "*" : dbName, user, _host.c_str(),
        password == nullptr ? "" : password, admin);
    _logger.say(LV_DETAIL, sql.c_str());
    execute(sql.c_str());
    execute("flush privileges;");
    _logger.say(LV_DETAIL, formatCString("= user created: %s", user));
  }
  return rc;
}

bool MySql::execute(const char *sql, const char **arguments) {
  bool rc = false;
  if (_statement != nullptr) {
    std::string sql2;
    try {
      _affectedRows = -1;
      if (arguments != nullptr) {
        sql2 = replacePositionalArguments(sql, '?', arguments);
        sql = sql2.c_str();
      }
      if (startsWith(sql, 6, "update", 6, true)) {
        _affectedRows = _statement->executeUpdate(sql);
      } else {
        _statement->execute(sql);
      }
      if (startsWith(sql, 6, "delete", 6, true)) {
        _affectedRows = _statement->getUpdateCount();
      }
      rc = true;
    } catch (sql::SQLException &e) {
      _logger.say(LV_ERROR,
          formatCString("%s (%d) %s", e.what(), e.getErrorCode(),
              e.getSQLState()));
    }
  }
  return rc;
}
int64_t MySql::lastInsertedId() {
  Result result(*this);
  query("SELECT LAST_INSERT_ID();", result, nullptr, 1);
  _lastInsertedId = atoll(result._rows[0]->asString(0));
  return _lastInsertedId;
}
bool MySql::loadDb(const char *database, const char *filename,
    Configuration &configuration) {
  bool rc = false;
  return rc;
}
bool MySql::query(const char *sql, Result &result, const char **arguments,
    int colCount) {
  bool rc = false;
  if (_statement == nullptr) {
    _logger.say(LV_ERROR,
        formatCString("missing connection: %s", _database.c_str()));
  } else {
    try {
      sql::ResultSet *res = _statement->executeQuery(sql);
      sql::ResultSetMetaData *meta = nullptr;
      result.clear();
      auto rowCount = res->rowsCount();
      if (rowCount > 0) {
        if (colCount < 0 || _buildMetaResult) {
          meta = res->getMetaData();
          result._nativeMeta = static_cast<void*>(meta);
          colCount = meta->getColumnCount();
        }
        result._colCount = colCount;
        result._rows.reserve(rowCount);
        while (res->next()) {
          RowResult *row = new RowResult(result);
          // column index is 1-based:
          for (int col = 1; col <= result.colCount(); col++) {
            auto value = res->getString(col);
            row->_values.push_back(value);
          }
          result._rows.push_back(row);
        }
      }
      if (_buildMetaResult) {
        if (_metaResult == nullptr) {
          _metaResult = new MetaResult(result);
        }
        _metaResult->_columnNames.clear();
        for (int col = 1; col <= result.colCount(); col++) {
          auto value = meta->getColumnName(col);
          _metaResult->_columnNames.push_back(value);
        }
      }
      res->close();
      delete res;
      rc = true;
    } catch (sql::SQLException &e) {
      _logger.say(LV_ERROR,
          formatCString("Connection failed: %s (%d) %s", e.what(),
              e.getErrorCode(), e.getSQLState()));
    }
  }
  return rc;
}

bool MySql::storeDb(const char *database, const char *filename,
    Configuration &configuration) {
  bool rc = false;
  LoginData login("mysql", configuration, database);
  Process process(&_logger);

  auto host = formatCString("-h%s", login._host);
  auto port = formatCString("-P%d", login._port);
  auto user = formatCString("-u%s", login._user);
  auto passwd = formatCString("-p%s", login._code);
  std::string output(filename);
  if (endsWith(filename, -1, ".gz")) {
    output.resize(output.size() - 3);
    output = "-r" + output;
    const char *argv1[] = { "/usr/bin/mysqldump",
        "--default-character-set=utf8mb4", "--single-transaction", host.c_str(),
        port.c_str(), user.c_str(), passwd.c_str(), database, output.c_str(),
        nullptr };
    const char *argv2[] = { "/usr/bin/gzip", output.c_str(), nullptr };
    process.execute(argv1);
    process.execute(argv2);
  } else if (endsWith(filename, -1, ".zst")) {
    output = formatCString("-o%s", filename);
    const char *argv1[] = { "/usr/bin/mysqldump",
        "--default-character-set=utf8mb4", "--single-transaction", host.c_str(),
        port.c_str(), user.c_str(), passwd.c_str(), database, output.c_str(),
        nullptr };
    const char *argv2[] =
        { "/usr/bin/zstd", "-z", "-", output.c_str(), nullptr };
    process.executeAndPipe(argv1, argv2);
  } else {
    output = "-r" + output;
    const char *argv1[] = { "/usr/bin/mysqldump",
        "--default-character-set=utf8mb4", "--single-transaction", host.c_str(),
        port.c_str(), user.c_str(), passwd.c_str(), output.c_str(), filename,
        nullptr };
    process.execute(argv1);
  }
  return rc;
}
bool MySql::summaryOf(const char *table, TableSummary &result) {
  bool rc = true;
  result.clear();
  Result resultQuery(*this);
  try {
    bool oldState = _buildMetaResult;
    _buildMetaResult = true;
    query(formatCString("SELECT COUNT(*) FROM %s;", table).c_str(), resultQuery,
        nullptr, 1);
    _buildMetaResult = oldState;
    result._rowCount = resultQuery._rows[0]->asInt(0);
    if (result._rowCount > 0) {
      query(formatCString("SELECT * FROM %s LIMIT 1;", table).c_str(),
          resultQuery);
      result._columns = _metaResult->_columnNames;
      result._colCount = resultQuery.colCount();
      result._columns.reserve(resultQuery._colCount);
    }
  } catch (sql::SQLException &e) {
    rc = false;
  }
  return rc;
}
} /* namespace cppknife */
