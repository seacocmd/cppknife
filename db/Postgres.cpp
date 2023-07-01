/*
 * Postgres.cpp
 *
 *  Created on: 12.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#if ! defined IGNORE_POSTGRES
#include "db.hpp"

namespace cppknife {

Postgres::Postgres(const char *database, const char *user, const char *password,
    Logger &logger, const char *host, int port) :
    SqlDriver(database, logger), _host(host), _port(port), _database(database), _user(
        user), _password(password), _connection(nullptr) {
}

Postgres::~Postgres() {
  close();
}

std::vector<std::string> Postgres::allDbs(bool withSystemTables) {
  std::vector<std::string> rc;
  Result result(*this);
  query("SELECT datname FROM pg_database order by datname;", result, nullptr,
      1);
  rc.reserve(result._rows.size());
  for (auto row : result._rows) {
    auto table = row->asString(0);
    if (withSystemTables
        || (strcmp(table, "postgres") != 0 && strcmp(table, "template1") != 0
            && strcmp(table, "template0") != 0)) {
      rc.push_back(table);
    }
  }
  return rc;
}
std::vector<std::string> Postgres::allTables() {
  std::vector<std::string> rc;
  Result result(*this);
  query(
      "SELECT tablename FROM pg_catalog.pg_tables WHERE schemaname!='pg_catalog' AND schemaname!='information_schema';",
      result, nullptr, 1);
  rc.reserve(result._rows.size());
  for (auto row : result._rows) {
    rc.push_back(row->asString(0));
  }
  return rc;
}
void Postgres::close() {
  if (_connection != nullptr) {
    //_connection->close();
    delete _connection;
    _connection = nullptr;
  }
}

bool Postgres::connect() {
  bool rc = false;
  try {
    close();
    /* Create a connection */
    auto connectString =
        formatCString(
            "user=%s password=%s host=%s port=%d dbname=%s target_session_attrs=read-write",
            _user.c_str(), _password.c_str(), _host.c_str(), _port,
            _database.c_str());
    _connection = new pqxx::connection(connectString.c_str());
    if (_connection == nullptr || !_connection->is_open()) {
      _logger.say(LV_ERROR,
          formatCString("cannot connect to %s", connectString.c_str()));
    } else {
      rc = true;
    }
  } catch (std::exception &e) {
    _logger.say(LV_ERROR, formatCString("Connection failed: %s", e.what()));
  }
  return rc;
}

bool Postgres::createDbAndUser(const char *dbName, const char *user,
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

bool Postgres::execute(const char *sql, const char **arguments) {
  bool rc = false;
  if (!_connection->is_open()) {
    _logger.say(LV_ERROR,
        formatCString("missing connection: %s", _database.c_str()));
  } else {
    try {
      _affectedRows = -1;
      pqxx::nontransaction transaction(*_connection);
      pqxx::result result;
      if (arguments == nullptr) {
        result = transaction.exec(sql);
      } else {
        result = transaction.exec(
            replacePositionalArguments(sql, '$', arguments));
      }
      transaction.commit();
      _affectedRows = result.affected_rows();
      _logger.say(LV_FINE, sql);
      rc = true;
    } catch (std::exception &e) {
      _logger.say(LV_ERROR, formatCString("Postgres::execute: %s", e.what()));
    }
  }
  return rc;
}
int64_t Postgres::lastInsertedId() {
  Result result(*this);
  query("SELECT LASTVAL();", result, nullptr, 1);
  _lastInsertedId = atoll(result._rows[0]->asString(0));
  return _lastInsertedId;
}
bool Postgres::loadDb(const char *database, const char *filename,
    Configuration &configuration) {
  bool rc = false;
  LoginData login("postgres", database);
  return rc;
}
bool Postgres::query(const char *sql, Result &result, const char **arguments,
    int colCount) {
  bool rc = false;
  if (!_connection->is_open()) {
    _logger.say(LV_ERROR,
        formatCString("missing connection: %s", _database.c_str()));
  } else {
    try {
      pqxx::result nativeResult;
      if (arguments == nullptr) {
        pqxx::nontransaction transaction(*_connection);
        nativeResult = transaction.exec(sql);
        transaction.commit();
        _logger.say(LV_FINE,
            formatCString("%d row(s): %s", nativeResult.size()));
      } else {
        _connection->prepare("", sql);
        pqxx::nontransaction transaction(*_connection);
        nativeResult = transaction.exec_prepared("", arguments);
      }
      result.clear();
      /* List down all the records */
      for (pqxx::result::const_iterator it = nativeResult.begin();
          it != nativeResult.end(); ++it) {
        RowResult *row = new RowResult(result);
        result._rows.push_back(row);
        if (colCount <= 0) {
          result._colCount = colCount = nativeResult.columns();
        }
        row->_values.reserve(colCount);
        for (int col = 0; col < colCount; col++) {
          row->_values.push_back(it[col].c_str());
        }
      }
      rc = true;
    } catch (std::exception &e) {
      _logger.say(LV_ERROR, formatCString("SQL-Error: %s", e.what()));
    }
  }
  return rc;
}

bool Postgres::storeDb(const char *database, const char *filename,
    Configuration &configuration) {
  bool rc = false;
  LoginData login("postgres", database);
  return rc;
}
bool Postgres::summaryOf(const char *table, TableSummary &result) {
  bool rc = true;
  result.clear();
  Result resultQuery(*this);
  try {
    query(formatCString("SELECT COUNT(*) FROM %s;", table).c_str(), resultQuery,
        nullptr, 1);
    result._rowCount = resultQuery._rows[0]->asInt(0);
    if (result._rowCount > 0) {
      query(formatCString("SELECT * FROM %s LIMIT 1;", table).c_str(),
          resultQuery);
      result._colCount = resultQuery.colCount();
      result._columns.reserve(resultQuery._colCount);
    }
  } catch (std::exception &e) {
    rc = false;
  }
  return rc;
}
} /* namespace cppknife */
#endif /* ! defined IGNORE_POSTGRES */
