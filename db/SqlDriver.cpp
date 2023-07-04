/*
 * SqlDriver.cpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "db.hpp"

namespace cppknife {

std::string SqlDriver::_default_driver = "mysql";

TableSummary::TableSummary() :
    _rowCount(0), _colCount(0) {
}
void TableSummary::clear() {
  _rowCount = 0;
  _colCount = 0;
}

TableMeta::TableMeta(SqlDriver &driver) :
    _driver(driver), _columns() {
}
TableMeta::~TableMeta() {
  clear();
}

void TableMeta::clear() {
  for (const auto& [key, value] : _columns) {
    delete value;
  }
  _columns.clear();
}

MetaResult::MetaResult(Result &parent) :
    _columnNames(), _parent(parent) {

}
MetaResult::~MetaResult() {
}

int MetaResult::indexOf(const char *column) const {
  auto it = std::find(_columnNames.cbegin(), _columnNames.cend(), column);
  int rc = -1;
  if (it != _columnNames.cend()) {
    rc = it - _columnNames.cbegin();
  }
  return rc;
}

RowResult::RowResult(Result &parent) :
    _values(), _parent(parent) {
}
RowResult::~RowResult() {
}
const char* RowResult::asString(int index) const {
  const char *rc = nullptr;
  if (index >= 0 && static_cast<size_t>(index) < _values.size()) {
    rc = _values[index].c_str();
  }
  return rc;
}
const char* RowResult::asString(const char *column) const {
  int index = _parent.indexOf(column);
  const char *rc = asString(index);
  return rc;
}
int RowResult::asInt(int index) const {
  const char *value = asString(index);
  int rc = -1;
  if (value != nullptr) {
    rc = atoi(value);
  }
  return rc;
}
int RowResult::asInt(const char *column) const {
  int index = _parent.indexOf(column);
  int rc = asInt(index);
  return rc;
}

Result::Result(SqlDriver &driver) :
    _driver(driver), _meta(nullptr), _rows(), _colCount(0) {
}
Result::~Result() {
  clear();
}
void Result::clear() {
  for (auto row : _rows) {
    delete row;
  }
  _rows.resize(0);
}

LoginData::LoginData(const char *driver, const char *database) :
    _driver(driver), _db(database), _user(nullptr), _code(nullptr), _host(
        nullptr), _port(0) {
}
LoginData::LoginData(const char *driver, Configuration &configuration,
    const char *database, bool readOnly) :
    _driver(driver), _db(database), _user(nullptr), _code(nullptr), _host(
        nullptr), _port(0) {
  if (!fromConfiguration(configuration, readOnly)) {
    throw InternalError(
        "LoginData::LoginData(): wrong database configuration: ", _driver);
  }
}
bool LoginData::fromConfiguration(Configuration &configuration, bool readOnly) {
  _user =
      configuration.asString(
          formatCString(readOnly ? "%s.ro.admin.name" : "%s.admin.name",
              _driver).c_str());
  _code =
      configuration.asString(
          formatCString(readOnly ? "%s.ro.admin.code" : "%s.admin.code",
              _driver).c_str());
  _host = configuration.asString(formatCString("%s.host", _driver).c_str());
  _port = configuration.asNat(formatCString("%s.port", _driver).c_str());
  return _user != nullptr && _code != nullptr && _host != nullptr && _port > 0;
}

SqlDriver::SqlDriver(const char *tableName, Logger &logger) :
    _affectedRows(-1), _databaseName(tableName), _logger(logger), _lastInsertedId(
        0) {
}

SqlDriver::~SqlDriver() {
}

std::string SqlDriver::replacePositionalArguments(const char *sql,
    char parameterChar, const char **parameters) {
  char internalBuffer[0x10000];
  char *buffer = internalBuffer;
  int parameterCount = 0;
  const char *parameter = parameters[0];
  size_t sqlLength = strlen(sql);
  size_t length = 0;
  while ((parameter = parameters[parameterCount]) != nullptr) {
    length = strlen(parameter);
    sqlLength += 2 + length + countCString(parameter, length, "'", 1);
    parameterCount++;
  }
  sqlLength += 5;
  if (sqlLength >= sizeof internalBuffer) {
    buffer = new char[sqlLength];
  }
  const char *ptr = sql;
  const char *last = sql;
  char *endOfBuffer = buffer;
  int ixParameter = -1;
  do {
    char cc = 0;
    while ((cc = *ptr) != '\'' && cc != parameterChar && cc != '\0') {
      ptr++;
    }
    if (cc == '\0') {
      length = ptr - last;
      memcpy(endOfBuffer, last, length + 1);
      endOfBuffer += length;
      break;
    }
    if (cc == '\'') {
      // search end of string (contains no parameter):
      while ((ptr = strchr(ptr + 1, cc)) != nullptr) {
        // escaped (doubled) tick?
        if (ptr[1] == cc) {
          ptr++;
        } else {
          // includes tick:
          ptr++;
          break;
        }
      }
    } else {
      // parameter found:
      ixParameter++;
      length = ptr - last;
      memcpy(endOfBuffer, last, length);
      endOfBuffer += length;
      last = ptr + 1;
      if (cc == '$') {
        ixParameter = (ptr[1] - '0');
        if (ixParameter < 0 || ixParameter > 9) {
          throw InternalError(
              formatCString(
                  "SqlDriver::replacePositionalArguments(): wrong positional argument: $%c %s",
                  ptr[1], sql));
        }
        last++;
        if (ptr[2] >= '0' && ptr[2] <= '9') {
          ixParameter = ixParameter * 10 + ptr[2] - '0';
          last++;
        }
        // 0-based arguments:
        ixParameter--;
      }
      if (ixParameter < 0) {
        throw InternalError(
            formatCString(
                "SqlDriver::replacePositionalArguments(): wrong positional parameter: $0: %s",
                ixParameter + 1, parameterCount, sql));
      }
      if (ixParameter >= parameterCount) {
        throw InternalError(
            formatCString(
                "SqlDriver::replacePositionalArguments(): too few arguments (%d/%d): %s",
                ixParameter + 1, parameterCount, sql));
      }
      length = strlen(parameter = parameters[ixParameter]);
      bool hasDelimiter = !isNat(parameter, length);
      if (hasDelimiter) {
        *endOfBuffer++ = '\'';
      }
      while ((ptr = strchr(parameter, '\'')) != nullptr) {
        length = ptr - parameter + 1;
        memcpy(endOfBuffer, parameters[ixParameter], length);
        endOfBuffer += length;
        // duplicate the tick:
        *endOfBuffer++ = '\'';
        parameter = ptr + 1;
      }
      length = strlen(parameter);
      if (length > 0) {
        memcpy(endOfBuffer, parameter, length);
        endOfBuffer += length;
      }
      if (hasDelimiter) {
        *endOfBuffer++ = '\'';
      }
      ptr = last;
    }
  } while (true);
  std::string rc(buffer, endOfBuffer - buffer);
  if (buffer != internalBuffer) {
    delete buffer;
    buffer = nullptr;
  }
  return rc;
}

} /* namespace cppknife */
