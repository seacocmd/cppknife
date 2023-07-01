/*
 * LineList.h
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_LINELIST_HPP_
#define TEXT_LINELIST_HPP_

namespace cppknife {

class Configuration;
/**
 * Manages a list of strings.
 */
enum ChangeType {
  CT_UNDEF, CT_UNCHANGED, CT_CHANGED, CT_INSERTED, CT_APPENDED
};
class SearchExpression;

class LineList;
class BufferPosition {
public:
  size_t _lineIndex;
  size_t _columnIndex;
public:
  BufferPosition() :
      _lineIndex(0), _columnIndex(0) {
  }
  BufferPosition(size_t line, size_t column);
public:
  inline BufferPosition& set(size_t line, size_t column) {
    _lineIndex = line;
    _columnIndex = column;
    return *this;
  }
};
/**
 * Stores the result of a search in a buffer.
 */
class SearchResult {
  friend SearchExpression;
public:
  bool _found;
  BufferPosition _position;
  size_t _length;
public:
  SearchResult();
  ~SearchResult();
public:
  void clear();
};

class SearchExpression {
  friend SearchResult;
  friend LineList;
protected:
  bool _isRegExpr;
  bool _ignoreCase;
  /// for simple strings: if <em>true</em> there are two meta characters: '^': begin of line '$': end of line
  bool _knowsMetaCharacters;
  /// for simple strings only: the pattern must lay at the begin of the line.
  bool _beginOfLine;
  /// for simple strings only: the pattern must lay at the end of the line.
  bool _endOfLine;
  bool _backwards;
  bool _inline;
  std::regex *_regExpr;
  std::string _pattern;
  std::string _flags;
public:
  SearchExpression(const char *pattern = nullptr, bool isRegExpr = false,
      const char *flags = nullptr);
  ~SearchExpression();
public:
  /**
   * Returns whether the search expression is defined (has a pattern).
   */
  inline bool isDefined() const {
    return !_pattern.empty();
  }
  void handleFlags(const char *flags);
  void handlePattern(bool isRegExpr);
  /**
   * Returns the search pattern as string.
   * @return the search pattern as string.
   */
  inline const std::string& pattern() const {
    return _pattern;
  }
  /**
   * Search the intrinsic pattern in a given text.
   * @param text In that text will be searched
   * @return <em>true</em>The search was successful.
   */
  inline bool search(const char *text) {
    return std::regex_search(text, *_regExpr);
  }
  /**
   * Returns the compiled regular expression.
   * @return <em>nullptr</em>: there is no regular expression. Otherwise: the regular expression.
   */
  inline const std::regex* regExpr() const {
    return _regExpr;
  }
  /**
   * Search the intrinsic pattern in a given buffer.
   * @param buffer The buffer to inspect.
   * @param[out] result The search result is stored here.
   * @param setPosition <em>true</em>: the current position in the buffer will be changed.
   * @return <em>true</em>The search was successful.
   */
  bool search(LineList &buffer, SearchResult &result, bool setPosition = true);
  /**
   * Sets the instance.
   * @param pattern The pattern to search.
   * @param isRegExpr <em>true</em>: the pattern is a regular expression.
   * @param flags A string with flags: 'i': ignore case 'M': known meta character (for simple strings)
   */
  void set(const char *pattern, bool isRegExpr, const char *flags = nullptr);
};

class LineList {
  friend SearchExpression;
public:
  const static int END_OF_LINE = 0x7fffffff;
  const static int END_OF_FILE = 0x7fffffff;
protected:
  std::vector<std::string> _lines;
  BufferPosition _position;
  BufferPosition _mark;
  BufferPosition _startLastHit;
  std::string _lastHit;
  bool _hasChanged;
  std::string _currentFilename;
  std::string _name;
  Logger &_logger;
public:
  LineList(size_t startSize = 100, Logger *logger = nullptr);
  virtual ~LineList();
  LineList(const LineList &other);
  LineList& operator=(const LineList &other);
public:
  /**
   * Replaces a part of a line in the line list.
   * The line to modify is the first line matching a given pattern.
   * If the pattern is not found: the pattern <em>anchor</em> is searched.
   * If the anchor is found the <em>replacement</em> is inserted depending on <em>aboveAnchor</em>.
   * If the anchor is not found the <em>replacement</em> is inserted as last line.
   * Example:<pre>
   * pattern: "max-memory="
   * replacement: "max-memory=512K"
   * anchor: "#.*max-memory"
   * </pre>
   * @param pattern The pattern of the line to modify as regular expression.
   * @param replacement The new value of the line after modifying.
   * @param anchor A pattern for finding the insertion position if <em>pattern</em> is not found.
   * May be <em>nullptr</em>.
   * @param aboveAnchor <em>true</em>: the insertion position is above the anchor, otherwise below.
   * @return CT_UNCHANGED: the line matching <em>pattern</em> is exactly <em>replacement</em>.
   *   CT_CHANGED: the <em>pattern</em> has been found and the line is modified.
   *   CT_INSERTED: the <em>pattern</em> has not been found and the <em>anchor</em> has been found.
   *   CT_APPENDED: the <em>pattern</em> has not been found and the <em>anchor</em> has not been found.
   */
  ChangeType adapt(const std::regex &pattern, const char *replacement,
      const std::regex *anchor = nullptr, bool aboveAnchor = false);

  /**
   * Replaces a part of a line in the line list.
   * The line to modify is the first line matching a given pattern.
   * If the pattern is not found: the pattern <em>anchor</em> is searched.
   * If the anchor is found the <em>replacement</em> is inserted depending on <em>aboveAnchor</em>.
   * If the anchor is not found the <em>replacement</em> is inserted as last line.
   * Example:<pre>
   * pattern: "max-memory="
   * replacement: "max-memory=512K"
   * anchor: "#.*max-memory"
   * </pre>
   * @param pattern The pattern of the line to modify as string.
   * @param replacement The new value of the line after modifying.
   * @param anchor A pattern for finding the insertion position if <em>pattern</em> is not found.
   * @param aboveAnchor <em>true</em>: the insertion position is above the anchor, otherwise below.
   * @return CT_UNCHANGED: the line matching <em>pattern</em> is exactly <em>replacement</em>.
   *   CT_CHANGED: the <em>pattern</em> has been found and the line is modified.
   *   CT_INSERTED: the <em>pattern</em> has not been found and the <em>anchor</em> has been found.
   *   CT_APPENDED: the <em>pattern</em> has not been found and the <em>anchor</em> has not been found.
   */
  ChangeType adapt(const char *pattern, const char *replacement,
      const char *anchor = nullptr, bool aboveAnchor = false);
  /**
   * Adapts some variables in file driven by data in a configuration file.
   * @param configuration Contains the data to search/replace.
   * @param logger The logging handler.
   * @param aboveAnchor The standard behavior for insertion.
   * @return <em>true</em>At least one change has been done.
   */
  bool adaptFromConfiguration(Configuration &configuration, Logger &logger,
      bool aboveAnchor = false);
  void clear() {
    _lines.clear();
  }
  /**
   * Returns the lines (not changeable).
   */
  const std::vector<std::string>& constLines() const {
    return _lines;
  }
  /**
   * Copies a range into a list of lines.
   * @param[out] target The result vector.
   * @param start The start position.
   * @param end The end position.
   * @param excluding <em>true</em> the end position is not part of the result.
   * @return <em>target</em> (for chaining).
   */
  std::vector<std::string>& copyRange(std::vector<std::string> &target,
      const BufferPosition &start, const BufferPosition &end, bool excluding =
          true);
  /**
   * Deletes the text from a given start position to a given end position (excluding).
   * @param start The start position.
   * @param end The end position
   */
  void deleteRange(BufferPosition start, BufferPosition end);
  /**
   * Returns whether the current position is at the buffer end.
   */
  inline bool endOfData() const {
    return _position._lineIndex >= _lines.size();
  }
  /**
   * Returns the name of the file.
   */
  inline const std::string& filename() const {
    return _currentFilename;
  }
  /**
   * Returns the first line matching a regular expression.
   * @param regExpr The search patterns as regular expression.
   * @param start the first line index to search.
   * @return -1: not found
   *   Otherwise: the index of the line (behind <em>start</em>).
   */
  int find(const std::regex &regExpr, size_t start = 0);
  /**
   * Returns the first line matching a regular expression.
   * @param regExpr The search patterns as string.
   * @param start the first line index to search.
   * @return -1: not found
   *   Otherwise: the index of the line (behind <em>start</em>).
   */
  int find(const char *regExpr, size_t start = 0);
  /**
   * Compares two lists and returns the index of the first different line.
   * @param other The other list to compare.
   * @param start The comparison starts at that position.
   * @param[out] differentLength <em>true</em>: sizes of the lists are different
   * @return -1: both lists are equal (between start and end).<br>
   *   Otherwise: the index of the first different line
   */
  int indexOfFirstDifference(const LineList &other, int start = 0,
      bool *differentLength = nullptr);
  /**
   * Returns the lines (changeable).
   */
  std::vector<std::string>& lines() {
    return _lines;
  }
  /**
   * Returns whether there was a modification of the lines.
   */
  bool hasChanged() const {
    return _hasChanged;
  }
  /**
   * Inserts a given list of lines at a given position.
   * @param position The text is inserted here. If the position is behind the end of buffer the text is appended.
   * @param lines The lines to insert.
   */
  void insert(const BufferPosition &position,
      const std::vector<std::string> &lines);
  /**
   * Inserts a given text at a given position.
   * @param position The text is inserted here. If the position is behind the end of buffer the text is appended.
   * @param text The text to insert.
   */
  void insert(const BufferPosition &position, const char *text);
  /**
   * Returns the found text of the last search.
   */
  inline const std::string& lastHit() const {
    return _lastHit;
  }
  /**
   * Returns the logger.
   */
  inline Logger& logger() {
    return _logger;
  }
  /**
   * Returns the current position.
   * @param[out] position The current position.
   * @return <em>position</em>: for chaining
   */
  inline BufferPosition& mark(BufferPosition &position) {
    position = _mark;
    return position;
  }
  /**
   * Returns the name of the instance.
   */
  inline const std::string& name() const {
    return _name;
  }
  /**
   * Returns the current position.
   * @param[out] position The current position.
   * @return <em>position</em>: for chaining
   */
  inline BufferPosition& position(BufferPosition &position) {
    position = _position;
    return position;
  }
  /**
   * Reads the content of a file into the instance.
   * @param filename The name of the file to read.
   * @param stripNewline <em>true</em>: the "\n" will be removed from the lines.
   * @return <em>true</em>: success.
   */
  bool readFromFile(const char *filename, bool stripNewline = false);
  /**
   * Replaces a pattern by a replacement.
   * @param searchExpression Defines the search pattern.
   * @param replacement The replacement string. May contain back references.
   * @param count The maximal count of replacements in each processed line. If <em>-1</em> not limit is given.
   * @param start <em>nullptr</em> or the begin of the range to process.
   * @param end <em>nullptr</em> or the end of the range to process.
   * @param filter <em>nullptr></em> or a search expression: A hit is found only if that search expression
   *  matches the processed line.
   * @param patternBackreference <em>nullptr</em> or a regular expression specifying a back reference.
   *    Must contain a group with a number, e.g. <em>std::regex pattern("!(\\d+)")</em>.
   * @return The number of replacements.
   */
  int replace(const SearchExpression &searchExpression, const char *replacement,
      int count = -1, const BufferPosition *start = nullptr,
      const BufferPosition *end = nullptr, const SearchExpression *filter =
          nullptr, const std::regex *patternBackreference = nullptr);
  /**
   * Replaces a pattern by a replacement in a given line.
   * @param[in out] line The line to process.
   * @param searchExpression Defines the search pattern.
   * @param replacement The replacement string. May contain back references.
   * @param count The maximal count of replacements in each processed line. If <em>-1</em> not limit is given.
   * @param patternBackreference <em>nullptr</em> or a regular expression specifying a back reference.
   *    Must contain a group with a number, e.g. <em>std::regex pattern("!(\\d+)")</em>.
   * @return The number of replacements.
   */
  static int replaceString(std::string &line,
      const SearchExpression &searchExpression, const char *replacement,
      int count = -1, const std::regex *patternBackreference = nullptr);
  /**
   * Searches a pattern from the current position.
   * @param searchExpression The pattern to search: a simple string or a regular expression.
   * @param[out] result The result of the search.
   * @param setPosition <em>true</em>The current position will be changed.
   * @return <em>true</em>The pattern has been found.
   */
  bool search(const SearchExpression &searchExpression, SearchResult &result,
      bool setPosition);
protected:
  bool searchBackwardsOneLine(size_t ixColumn, size_t ixLine,
      std::regex &regExpr, SearchResult &result);
  bool searchBackwardsRegExpr(std::regex &regExpr, SearchResult &result,
      bool setPosition, bool endOfLine, bool inlineOnly, const char *flags);
  bool searchRegExpr(std::regex &regExpr, SearchResult &result,
      bool setPosition, bool beginOfLine, bool inlineOnly, const char *flags);
public:
  bool searchSimpleString(const char *pattern, int length, SearchResult &result,
      bool setPosition = false, bool beginOfLine = false,
      bool endOfLine = false);
  /**
   * Defines the filename of the buffer.
   * @param filename The name of the file assigned to the buffer.
   */
  inline void setFilename(const char *filename) {
    _currentFilename = filename;
  }
  /**
   * Sets the lines.
   * @param lines The new contents.
   * @param append <em>true</em>: the lines will be appended.
   */
  inline void setLines(const std::vector<std::string> &lines, bool append =
      false) {
    if (append) {
      _lines.insert(_lines.end(), lines.begin(), lines.end());
    } else {
      _lines = lines;
    }
  }
  /**
   * Sets the mark (a special position).
   * @param position The new position.
   * @return the current position (may be corrected).
   */
  BufferPosition& setMark(const BufferPosition &position);
  /**
   * Sets the name of the instance.
   */
  inline void setName(const char *name) {
    _name = name;
  }
  /**
   * Sets the current position.
   * @param position The new position.
   * @return the current position (may be corrected).
   */
  BufferPosition& setPosition(const BufferPosition &position);
  /**
   * Sets the current position.
   * @param ixLine The new current line. If &lt; 0 current line is not changed.
   * @param ixColumn The new current column. If &lt; 0 current column is not changed.
   * @return the current position (may be corrected).
   */
  BufferPosition& setPosition(int ixLine, int ixColumn = -1);
  /**
   * Returns the current position.
   * @param[out] position The current position.
   * @return <em>position</em>: for chaining
   */
  inline BufferPosition& startLastHit(BufferPosition &position) {
    position = _startLastHit;
    return position;
  }
  /**
   * Writes the lines into a file.
   * @param filename The name of the file to write.
   * @param force <em>false</em>: the file will be written only if data has changed
   * @param append <em>true</em>: the data will be appended to the file. This implies <em>force=true</em>.
   * @return <em>true</em>Success.
   */
  bool writeToFile(const char *filename, bool force = false,
      bool append = false);
};

} /* namespace cppknife */

#endif /* TEXT_LINELIST_HPP_ */
