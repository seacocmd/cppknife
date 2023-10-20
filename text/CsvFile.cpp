/*
 * CsvFile.cpp
 *
 *  Created on: 30.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
/// Implements a reader for CSV files.
namespace cppknife {

CsvRow::CsvRow(CsvFile &parent, const char *line, int colCount) :
    _parent(parent), _columns() {
  if (line != nullptr) {
    read(line, colCount);
  }
}
void CsvRow::addColumn(const char *contents) {
  _columns.push_back(contents);
}

double CsvRow::asDouble(size_t columnIndex, double defaultValue) const {
  const char *contents = rawColumn(columnIndex);
  while (*contents == ' ') {
    contents++;
  }
  double rc = defaultValue;
  if (contents != nullptr) {
    char *endPtr;
    rc = strtod(contents, &endPtr);
    while (*endPtr == ' ') {
      endPtr++;
    }
    if ((rc == 0.0 && *contents != '0') || *endPtr != '\0') {
      rc = defaultValue;
    }
  }
  return rc;
}
int CsvRow::asInt(size_t columnIndex, int defaultValue) const {
  const char *contents = rawColumn(columnIndex);
  int rc = defaultValue;
  if (contents != nullptr) {
    while (*contents == ' ') {
      contents++;
    }
    rc = atoi(contents);
    if (rc == 0 && *contents != '0') {
      rc = defaultValue;
    }
  }
  return rc;
}

void CsvRow::clone(const CsvRow &source) {
  _columns = source._columns;
}
std::string CsvRow::pureColumn(size_t columnIndex) const {
  std::string rc;
  const char *contents =
      columnIndex >= _columns.size() ? nullptr : _columns[columnIndex].c_str();
  char cc;
  char delimiter1 = _parent._delimiter1;
  char delimiter2 = _parent._delimiter2;
  if (contents != nullptr) {
    rc.reserve(strlen(contents));
    char escChar = _parent._escChar;
    if (escChar != 0) {
      while ((cc = *contents++) != '\0') {
        if (cc != escChar) {
          rc += cc;
        } else {
          cc = *contents++;
          if (cc == '\0') {
            rc = escChar;
            break;
          }
        }
        switch (cc) {
        case 'n':
          rc += '\n';
          break;
        case 'r':
          rc += '\r';
          break;
        case 'v':
          rc += '\v';
          break;
        case 't':
          rc += '\t';
          break;
        default:
          rc += cc;
          break;
        }
      }
    } else {
      // Remove doubled delimiters:
      while ((cc = *contents++) != '\0') {
        if ((cc == delimiter1 || cc == delimiter2) && *contents == cc) {
          rc += cc;
          contents++;
        } else {
          rc += cc;
        }
      }
    }
  }
  return rc;
}
const char* CsvRow::rawColumn(size_t columnIndex) const {
  const char *rc =
      columnIndex >= _columns.size() ? nullptr : _columns[columnIndex].c_str();
  return rc;
}
void CsvRow::read(const char *line, int colCount) {
  _columns.reserve(colCount);
  char openDelimiter = '\0';
  char separator = _parent._separator;
  char delimiter1 = _parent._delimiter1;
  char delimiter2 = _parent._delimiter2;
  bool escChar = _parent._escChar;
  char cc;
  const char *start = line;
  char nextChar = *line++;
  bool hasDelimiter = false;
  if (nextChar == delimiter1 || nextChar == delimiter2) {
    start++;
    openDelimiter = nextChar;
    nextChar = *line++;
    hasDelimiter = true;
  }
  while ((cc = nextChar) != '\0') {
    nextChar = *line++;
    if (cc == delimiter1 || cc == delimiter2) {
      if (openDelimiter != '\0' && cc == openDelimiter
          && (nextChar == separator || nextChar == '\0')) {
        openDelimiter = '\0';
      }
    } else if (cc == escChar) {
      if (nextChar != '\0') {
        nextChar = *line++;
      }
    } else {
      if (cc == separator && openDelimiter == '\0') {
        // -2: line has eaten nextChar, ignore separator:
        int length = line - start - 2 - (hasDelimiter ? 1 : 0);
        // We do not store a trailing delimiter:
        std::string col(start, length);
        _columns.push_back(col);
        // line has eaten nextChar:
        start = line - 1;
        hasDelimiter = nextChar == delimiter1 || nextChar == delimiter2;
        if (hasDelimiter) {
          start++;
          openDelimiter = nextChar;
          nextChar = *line++;
        }
      }
    }
  }
  if (hasDelimiter) {
    std::string col(start, line - start - 2);
    _columns.push_back(col);
  } else {
    _columns.push_back(start);
  }
}

void CsvRow::write(FILE *fp) const {
  char separator = _parent._separator;
  char delimiter = _parent._delimiter1;
  for (size_t columnIndex = 0; columnIndex < _columns.size(); columnIndex++) {
    if (columnIndex > 0) {
      fputc(separator, fp);
    }
    auto data = rawColumn(columnIndex);
    if (strchr(data, separator) == nullptr
        && strchr(data, delimiter) == nullptr) {
      fwrite(data, 1, strlen(data), fp);
    } else {
      fputc(delimiter, fp);
      fwrite(data, 1, strlen(data), fp);
      fputc(delimiter, fp);
    }
  }
  fputc('\n', fp);
}

CsvFile::CsvFile(Logger &logger, const char *filename, char separator,
    char delimiter, char escChar) :
    LineReader(filename, logger), _separator(separator), _delimiter1(delimiter), _delimiter2(
        0), _escChar(escChar), _rows() {
  if (delimiter == AUTO_DELIMITER) {
    _delimiter1 = '"';
    _delimiter2 = '\'';
  }
  if (filename != nullptr) {
    read();
  }
}

CsvFile::~CsvFile() {
  for (auto row : _rows) {
    delete row;
  }
  _rows.clear();
}

void CsvFile::detectSeparator() {
  if (_cursorNextBlock == nullptr) {
    readBlock();
  }
  const char *ptr = _nextBlock.c_str();
  size_t size = _nextBlock.size();
  int tabs = 0;
  int semicolons = 0;
  int commas = 0;
  while (size-- > 0) {
    char cc = *ptr++;
    switch (cc) {
    case '\t':
      tabs++;
      break;
    case ',':
      commas++;
      break;
    case ';':
      semicolons++;
      break;
    }
  }
  if (tabs >= semicolons) {
    _separator = tabs >= commas ? '\t' : semicolons > commas ? ';' : ',';
  } else {
    _separator = semicolons >= commas ? ';' : ',';
  }
}

bool CsvFile::nextRow(CsvRow &row) {
  row._columns.clear();
  bool rc = false;
  if (!endOfFile()) {
    rc = true;
    auto line = nextLine();
    if (!line.empty()) {
      row.read(line.c_str(), row._columns.capacity());
    }
  }
  return rc;
}
bool CsvFile::openCsv(const char *filename, char separator, char delimiter,
    char escChar) {
  openFile(filename);
  _separator = separator;
  _delimiter1 = delimiter;
  if (delimiter == AUTO_DELIMITER) {
    _delimiter1 = '"';
    _delimiter2 = '\'';
  }
  _escChar = escChar;
  if (separator == AUTO_SEPARATOR) {
    detectSeparator();
  }
  bool rc = !endOfFile();
  return rc;
}

void CsvFile::read(const char *filename, int additionalRows) {
  if (filename != nullptr) {
    openFile(filename);
  }
  int lineCount = estimateLineCount();
  _rows.reserve(lineCount);

  int colCount = 20;
  std::string line;
  if (_separator == AUTO_DELIMITER) {
    detectSeparator();
    auto firstLines = lookahead(1);
    if (firstLines.size() > 0) {
      line = firstLines[0];
      colCount = countCString(line.c_str(), line.size(), &_separator, 1);
    }
  }
  while (!endOfFile()) {
    line = nextLine();
    auto row = new CsvRow(*this, line.c_str(), colCount);
    colCount = row->columnCount() + additionalRows;
    _rows.push_back(row);
  }
}
void CsvFile::write(const char *filename) {
  if (filename == nullptr) {
    filename = _filename.c_str();
  }
  FILE *fp = fopen(filename, "w");
  if (fp == nullptr) {
    throw cppknife::InternalError("cannot open: %s", filename);
  } else {
    for (auto row : _rows) {
      row->write(fp);
    }
    fclose(fp);
  }
}

}
/* namespace cppknife */
