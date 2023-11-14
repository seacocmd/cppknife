/*
 * CsvFile.hpp
 *
 *  Created on: 30.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_CSVFILE_HPP_
#define TEXT_CSVFILE_HPP_

namespace cppknife {
class CsvFile;
/**
 * @brief Stores one row of a CSV file.
 */
class CsvRow {
  friend CsvFile;
  CsvFile &_parent;
  std::vector<std::string> _columns;
  /// the row number (beginning with 0)
  int _rowIndex;
public:
  /**
   * Constructor.
   * @param parent The associated CsvFile instance.
   * @param line The raw row.
   * @param colCount The number of columns to reserve.
   */
  CsvRow(CsvFile &parent, const char *line = nullptr, int colCount = 20);
public:
  /**
   * Appends a new column to
   */
  void addColumn(const char *contents);
  /**
   * Returns the column value as float.
   * @param columnIndex The index of the column to return.
   * @param defaultValue That value is returned on index or data type error.
   * @return The column value as double or defaultValue on error.
   */
  double asDouble(size_t columnIndex, double defaultValue) const;
  /**
   * Returns the column value as integer.
   * @param columnIndex The index of the column.
   * @param defaultValue That value is returned on index or data type error.
   * @return The column value as integer or defaultValue on error.
   */
  int asInt(size_t columnIndex, int defaultValue) const;
  /**
   * Copies the data from source without changing the parent.
   * @param source The source to copy.
   */
  void clone(const CsvRow &source);
  /**
   * Returns the number of columns.
   */
  inline size_t columnCount() const {
    return _columns.size();
  }
  /**
   * Enlarges the capacity of the columns.
   * @param additionalColumns The new capacity is the current count of columns plus that amount.
   */
  inline void expandColumns(int additionalColumns) {
    _columns.reserve(_columns.size() + additionalColumns);
  }
  /**
   * Returns the given column without delimiter as interpreted text:
   * meta character (doubled delimiters or esc char sequences) are resolved.
   * Example: "a_""cat""" returns: a_"cat"
   * @param columnIndex The column index.
   * @return: "": column is wrong. Otherwise: the content of the column.
   */
  std::string pureColumn(size_t columnIndex) const;
  /**
   * Returns the column without delimiter but uninterpreted (with esc chars).
   * @param columnIndex The column index.
   * @return: "": column is wrong. Otherwise: the content of the column without delimiter.
   */
  const char* rawColumn(size_t columnIndex) const;
  /**
   * Reads the data from a file into the instance.
   * @param line The line to inspect.
   * @param colCount The number of columns.
   */
  void read(const char *line, int colCount = 20);
  /**
   * Returns the row position in the file (beginning with 0).
   * @return the row index
   */
  inline int rowIndex() const {
    return _rowIndex;
  }
  /**
   * Writes the whole row into a file.
   * @param fp The FILE instance to write.
   */
  void write(FILE *fp) const;
};

/**
 * @brief Manages a Comma Separated File (CSV).
 *
 * A CSV file is a plain text file divided in rows and columns.
 * Normally one row is one text line.
 * The columns are separated by one character named "separator".
 * To avoid ambiguities one column can be put into two delimiters: '"' or '\''.
 * There are two ways to handle a CSV file:
 * <ul><li>traversing row by row</li>
 * <li>parsing the whole file at the begin</li>
 * </ul>
 */
class CsvFile: public LineReader {
  friend CsvRow;
public:
  static const char AUTO_SEPARATOR = '\0';
  static const char AUTO_DELIMITER = '\0';
protected:
  char _separator;
  char _delimiter1;
  char _delimiter2;
  char _escChar;
  std::vector<CsvRow*> _rows;
public:
  /**
   * Constructor.
   * @param logger The logging manager.
   * @param filename <em>nullptr</em> or the name of the CSV file.
   * @param separator The separator of the columns. May be AUTO_SEPARATOR.
   * @param delimiter The delimiter to hide special characters. If <em>AUTO_DELIMITER</em> '"' and '\'' are used.
   * @param escChar 0 or a character that makes the following char to a meta char, e.g. '\n' for newline
   */
  CsvFile(Logger &logger, const char *filename = nullptr, char separator =
      AUTO_SEPARATOR, char delimiter = AUTO_DELIMITER, char escChar = 0);
  virtual ~CsvFile();
  /**
   * Returns a constant row.
   * @param rowIndex The index of the row.
   * @return <em>nullptr</em> (if invalid index) or the row.
   */
  inline const CsvRow* cRow(size_t rowIndex) const {
    return rowIndex >= _rows.size() ? nullptr : _rows[rowIndex];
  }
  /**
   * Returns the delimiter.
   */
  inline char delimiter() const {
    return _delimiter1;
  }
  /**
   * Tries to detect the separator from the first block.
   */
  void detectSeparator();
  /**
   * Returns the next row of the file.
   * @param row OUT: If result is true: The next row. Otherwise: unchanged.
   * @return <em>true</em>: a row has been found
   */
  bool nextRow(CsvRow &row);
  /**
   * Opens a CSV file for processing.
   * @param filename The filename of the CSV file.
   * @param separator The separator of the columns. May be AUTO_SEPARATOR.
   * @param delimiter The delimiter to hide special characters. If <em>AUTO_DELIMITER</em> '"' and '\'' are used.
   * @param escChar 0 or a character that makes the following char to a meta char, e.g. '\n' for newline
   * @return <em>true</em>: data is available.
   */
  bool openCsv(const char *filename, char separator = AUTO_SEPARATOR,
      char delimiter = AUTO_DELIMITER, char escChar = 0);
  /**
   * Reads the CSV file into the instance.
   * @param filename The filename. If <em>nullptr</em> the <em>_filename</em> is used.
   * @param additionalRows If not 0 the columns reserve more items (for efficient expansion).
   */
  void read(const char *filename = nullptr, int additionalRows = 0);
  /**
   * Returns a changable row.
   * @param rowIndex The index of the row.
   * @return <em>nullptr</em> (if invalid index) or the row.
   */
  inline CsvRow* row(size_t rowIndex) {
    return rowIndex >= _rows.size() ? nullptr : _rows[rowIndex];
  }
  /**
   * Returns the number of rows.
   * @return The number of rows.
   */
  inline size_t rowCount() const {
    return _rows.size();
  }
  /**
   * Returns the separator.
   */
  inline char separator() const {
    return _separator;
  }
  /**
   * Writes the CSV file.
   * @param filename The filename. If <em>nullptr</em> the <em>_filename</em> is used.
   */
  void write(const char *filename = nullptr);
};

} /* namespace cppknife */

#endif /* TEXT_CSVFILE_HPP_ */
