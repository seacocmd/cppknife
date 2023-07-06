/*
 * LineList.cpp
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"
#include "../os/os.hpp"

namespace cppknife {

BufferPosition::BufferPosition(size_t line, size_t column) :
    _lineIndex(line), _columnIndex(column) {
}

SearchExpression::SearchExpression(const char *pattern, bool isRegExpr,
    const char *flags) :
    _isRegExpr(isRegExpr), _ignoreCase(false), _knowsMetaCharacters(false), _beginOfLine(
        false), _endOfLine(false), _backwards(false), _inline(false), _regExpr(
        nullptr), _pattern(pattern == nullptr ? "" : pattern), _flags() {
  if (isRegExpr && pattern != nullptr) {
    _regExpr = new std::regex(pattern);
  }
  handleFlags(flags);
  handlePattern(isRegExpr);
}

SearchExpression::~SearchExpression() {
  delete _regExpr;
  _regExpr = nullptr;
}

void SearchExpression::handleFlags(const char *flags) {
  _inline = _backwards = _knowsMetaCharacters = _ignoreCase = false;
  char cc = 0;
  if (flags != nullptr) {
    _flags = flags;
    while ((cc = *flags++) != '\0' && !isspace(cc)) {
      switch (cc) {
      case 'L':
        _inline = true;
        break;
      case 'B':
        _backwards = true;
        break;
      case 'M':
        _knowsMetaCharacters = true;
        break;
      case 'i':
        _ignoreCase = true;
        break;
      default:
        break;
      }
    }
  }
}
void SearchExpression::handlePattern(bool isRegExpr) {
  if (!_pattern.empty()) {
    _beginOfLine = (isRegExpr || _knowsMetaCharacters) && _pattern[0] == '^';
    _endOfLine = (isRegExpr || _knowsMetaCharacters) && _pattern.back() == '$';
    if (!_isRegExpr) {
      static const std::regex metacharacters(R"([-+.^$()[\]{}|?*])");
      std::regex_replace(_pattern, metacharacters, "\\$&");
      if (_beginOfLine) {
        _pattern.erase(0, 1);
      }
      if (_endOfLine) {
        _pattern.erase(_pattern.size() - 2, 1);
      }
    }
    delete _regExpr;
    if (_ignoreCase) {
      _regExpr = new std::regex(_pattern, std::regex::icase);
    } else {
      _regExpr = new std::regex(_pattern);
    }
  }
}

void SearchExpression::set(const char *pattern, bool isRegExpr,
    const char *flags) {
  _pattern = pattern;
  _isRegExpr = isRegExpr;
  handleFlags(flags);
  handlePattern(isRegExpr);
}

SearchResult::SearchResult() :
    _found(false), _position(), _length(0) {
}
SearchResult::~SearchResult() {
}
void SearchResult::clear() {
  _found = false;
  _position._lineIndex = _position._columnIndex = _length = 0;
}

LineList::LineList(size_t startSize, Logger *logger) :
    _lines(), _position(), _startLastHit(), _lastHit(), _hasChanged(false), _currentFilename(), _logger(
        logger == nullptr ? *buildMemoryLogger() : *logger) {
  _lines.reserve(startSize);
}

LineList::~LineList() {
}

LineList::LineList(const LineList &other) :
    _lines(other._lines), _position(other._position), _mark(), _hasChanged(
        other._hasChanged), _currentFilename(other._currentFilename), _logger(
        other._logger) {
}

LineList& LineList::operator=(const LineList &other) {
  _lines = other._lines;
  _position = other._position;
  _hasChanged = other._hasChanged;
  _currentFilename = other._currentFilename;
  return *this;
}

ChangeType LineList::adapt(const std::regex &pattern, const char *replacement,
    const std::regex *anchor, bool aboveAnchor) {
  ChangeType rc = CT_UNDEF;
  int lineNo = 0;
  if ((lineNo = find(pattern)) >= 0) {
    if (strcmp(replacement, _lines[lineNo].c_str()) == 0) {
      rc = CT_UNCHANGED;
    } else {
      rc = CT_CHANGED;
      _lines[lineNo] = replacement;
      _hasChanged = true;
    }
  } else if (anchor != nullptr && (lineNo = find(*anchor)) >= 0) {
    rc = CT_INSERTED;
    if (!aboveAnchor) {
      lineNo++;
    }
    _lines.insert(_lines.begin() + lineNo, replacement);
    _hasChanged = true;
  } else {
    rc = CT_APPENDED;
    _lines.push_back(replacement);
  }
  return rc;
}

ChangeType LineList::adapt(const char *pattern, const char *replacement,
    const char *anchor, bool aboveAnchor) {
  std::regex anchor2(anchor == nullptr ? "" : anchor);
  ChangeType rc = adapt(std::regex(pattern), replacement,
      anchor == nullptr ? nullptr : &anchor2);
  return rc;
}

bool LineList::adaptFromConfiguration(Configuration &configuration,
    Logger &logger, bool aboveAnchor) {
  bool hasChanged = false;
  int withSpaces = 0;
  int withoutSpaces = 0;
  std::string name2;
  name2.reserve(100);
  auto names = configuration.names(nullptr, "^_anchor_.");
  for (auto name : names) {
    name2 = "^" + name + "=";
    if (find(name2.c_str()) >= 0) {
      withoutSpaces++;
    } else {
      name2 = "^" + name + "\\s+=";
      if (find(name2.c_str()) >= 0) {
        withSpaces++;
      }
    }
  }
  std::string assignment(withoutSpaces > withSpaces ? "=" : " = ");
  for (auto name : names) {
    name2 = "_anchor_." + name;
    const char *anchor = configuration.asString(name2.c_str(), nullptr);
    std::string pattern = "^" + name + "\\s*=";
    std::string replacement = name + assignment
        + configuration.asString(name.c_str(), "");
    hasChanged = true;
    switch (adapt(pattern.c_str(), replacement.c_str(), anchor, aboveAnchor)) {
    case CT_APPENDED:
      logger.say(LV_INFO, formatCString("= appended: %s", replacement.c_str()));
      break;
    case CT_CHANGED:
      logger.say(LV_INFO, formatCString("= changed: %s", replacement.c_str()));
      break;
    case CT_INSERTED:
      logger.say(LV_INFO, formatCString("= inserted: %s", replacement.c_str()));
      break;
    default:
    case CT_UNCHANGED:
      hasChanged = false;
      logger.say(LV_DETAIL,
          formatCString("= already correct: %s", replacement.c_str()));
      break;
    }
    if (hasChanged) {
      _hasChanged = true;
    }
  }
  return hasChanged;
}

std::vector<std::string>& LineList::copyRange(std::vector<std::string> &target,
    const BufferPosition &start, const BufferPosition &end, bool excluding) {
  size_t endLine = min(end._lineIndex, _lines.size());
  target.reserve(endLine - start._lineIndex + 1);
  size_t ixLine = start._lineIndex;
  size_t ixCol = 0;
  if (start._columnIndex > 0) {
    if (start._columnIndex < _lines[ixLine].size()) {
      if (end._columnIndex != 0 && ixLine == endLine) {
        ixCol = min(_lines[ixLine].size(), end._columnIndex + !excluding);
        target.push_back(
            _lines[ixLine].substr(start._columnIndex,
                ixCol - start._columnIndex));
      } else {
        target.push_back(_lines[ixLine].substr(start._columnIndex));
      }
    }
    ixLine++;
  }
  while (ixLine < endLine) {
    target.push_back(_lines[ixLine++]);
  }
  if (ixLine <= endLine && end._columnIndex != 0) {
    ixCol = min(_lines[ixLine].size(), end._columnIndex + !excluding);
    target.push_back(_lines[ixLine].substr(0, ixCol));
  }
  return target;
}
void LineList::deleteRange(BufferPosition start, BufferPosition end) {
  size_t length = 0;
  if (start._lineIndex > end._lineIndex
      || (start._lineIndex == end._lineIndex
          && start._columnIndex > end._columnIndex)) {
    BufferPosition tmp = start;
    start = end;
    end = tmp;
  }
  if (end._lineIndex < _lines.size()
      && end._columnIndex > (length = _lines[end._lineIndex].size())) {
    end._columnIndex = length;
  }
  if (start._lineIndex == end._lineIndex) {
    // delete inside one line:
    if (start._columnIndex < _lines.size()) {
      _lines[start._lineIndex].erase(start._columnIndex,
          end._columnIndex - start._columnIndex);
    }
  } else {
    int ixTop = -1;
    // Delete a part of the first line:
    if (start._columnIndex > 0) {
      _lines[start._lineIndex].erase(start._columnIndex);
      ixTop = start._lineIndex++;
    }
    // Delete whole lines:
    auto count = end._lineIndex - start._lineIndex;
    if (count > 0) {
      _lines.erase(_lines.begin() + start._lineIndex,
          _lines.begin() + end._lineIndex);
      end._lineIndex -= count;
    }
    // Delete a part of the last line:
    if (end._columnIndex > 0) {
      _lines[end._lineIndex].erase(0, end._columnIndex);
    }
    auto ixEnd = end._lineIndex;
    if (ixTop >= 0 && ixEnd == (size_t) ixTop + 1) {
      _lines[ixTop] += _lines[ixEnd];
      _lines.erase(_lines.begin() + ixEnd, _lines.begin() + ixEnd + 1);
    }
  }
}
int LineList::find(const std::regex &regExpr, size_t start) {
  int rc = -1;
  while (start < _lines.size()) {
    if (std::regex_search(_lines[start], regExpr)) {
      rc = start;
      break;
    }
    start++;
  }
  return rc;
}

int LineList::find(const char *regExpr, size_t start) {
  std::regex expr(regExpr);
  int rc = find(expr, start);
  return rc;
}

int LineList::indexOfFirstDifference(const LineList &other, int start,
    bool *differentLength) {
  int rc = -1;
  if (differentLength != nullptr) {
    *differentLength = false;
  }
  for (size_t ix = start; ix < _lines.size(); ix++) {
    if (ix >= other.constLines().size()) {
      if (differentLength != nullptr) {
        *differentLength = true;
      }
      rc = ix;
      break;
    }
    if (_lines[ix] != other.constLines()[ix]) {
      rc = ix;
      break;
    }
    if (rc && _lines.size() < other.constLines().size()) {
      rc = _lines.size();
      if (differentLength != nullptr) {
        *differentLength = true;
      }
    }
  }
  return rc;
}

void LineList::insert(const BufferPosition &position,
    const std::vector<std::string> &lines, bool addNewline) {
  if (position._lineIndex >= _lines.size()) {
    // Append all lines:
    _lines.insert(_lines.end(), lines.begin(), lines.end());
  } else {
    std::string top;
    std::string tail;
    auto ixInsert = position._lineIndex;
    auto line = _lines[ixInsert];
    if (position._columnIndex > 0) {
      top = line.substr(0, position._columnIndex);
    }
    if (position._columnIndex > 0 && position._columnIndex < line.size()) {
      tail = line.substr(position._columnIndex);
    }
    if (lines.size() == 1) {
      // only one line:
      _lines[ixInsert] = top + lines[0] + tail;
    } else {
      size_t firstLine = 0;
      size_t lastLine = lines.size() - 1;
      if (position._columnIndex > 0) {
        _lines[position._lineIndex] = top + lines[0];
        firstLine = 1;
      }
      auto needed = _lines.size() + lines.size() - firstLine;
      if (_lines.capacity() < needed) {
        _lines.reserve(needed);
      }
      if (lastLine > 0) {
        auto offset = position._columnIndex == 0 ? 0 : 1;
        _lines.insert(_lines.begin() + ixInsert + offset,
            lines.begin() + firstLine,
            lines.end());
      }
      // Add the tail to the last line:
      if (!tail.empty()) {
        if (addNewline) {
          _lines.insert(_lines.begin() + ixInsert + lastLine + 1, tail);
        } else {
          _lines[ixInsert + lastLine] += tail;
        }
      }
    }
  }
}

void LineList::insert(const BufferPosition &position, const char *text) {
  auto lines = splitCString(text, "\n");
  insert(position, lines);
}

int LineList::replace(const SearchExpression &searchExpression,
    const char *replacement, int count, const BufferPosition *start,
    const BufferPosition *end, const SearchExpression *filter,
    const std::regex *patternBackreference) {
  int rc = 0;
  BufferPosition start0(0, 0);
  if (start == nullptr) {
    start = &start0;
    if (searchExpression._inline) {
      start0.set(_position._lineIndex, 0);
    }
  }
  BufferPosition end0(_lines.size(), 0);
  if (end != nullptr) {
    end0 = *end;
  }
  if (searchExpression._inline) {
    end0.set(_position._lineIndex + 1, 0);
  } else if (end0._lineIndex < _lines.size()
      && _lines[end0._lineIndex].size() <= end0._columnIndex) {
    end0._lineIndex++;
    end0._columnIndex = 0;
  }
  size_t ixLine = start->_lineIndex;
  size_t ixEndLine = end0._lineIndex;
  if (ixLine < _lines.size()) {
    std::string line;
    std::string prefix;
    std::string suffix;
    size_t ixEnd = _lines[ixLine].size();
    if (filter == nullptr
        || std::regex_search(_lines[ixLine], *filter->_regExpr)) {
      if (ixEndLine == ixLine && end0._columnIndex < ixEnd) {
        ixEnd = end0._columnIndex;
        suffix = _lines[ixLine].substr(ixEnd);
      }
      // Process the part of the start line:
      if (start->_columnIndex > 0) {
        prefix = _lines[ixLine].substr(0, start->_columnIndex);
        line = _lines[ixLine].substr(start->_columnIndex,
            ixEnd - start->_columnIndex);
        int rc2 = replaceString(line, searchExpression, replacement, count,
            patternBackreference);
        rc += rc2;
        if (rc2 > 0) {
          _lines[ixLine] = prefix + line + suffix;
        }
        ixLine++;
      }
    }
    // Process whole lines:
    while (ixLine < _lines.size() && ixLine < ixEndLine) {
      if (filter != nullptr
          && !std::regex_search(_lines[ixLine], *filter->_regExpr)) {
        ixLine++;
        continue;
      }
      int rc2 = replaceString(_lines[ixLine], searchExpression, replacement,
          count, patternBackreference);
      rc += rc2;
      ixLine++;
    }
    // Process the part of the end line:
    if (ixLine < _lines.size() && ixLine == ixEndLine
        && (ixEnd = end0._columnIndex) > 0
        && (filter == nullptr
            || std::regex_search(_lines[ixLine], *filter->_regExpr))) {
      line = _lines[ixLine].substr(0, ixEnd);
      suffix = _lines[ixLine].substr(ixEnd);
      int rc2 = replaceString(line, searchExpression, replacement, count,
          patternBackreference);
      if (rc2 > 0) {
        rc += rc2;
        _lines[ixLine] = line + suffix;
      }
    }
  }
  return rc;
}

int LineList::replaceString(std::string &line,
    const SearchExpression &searchExpression, const char *replacement,
    int count, const std::regex *patternBackreference) {
  int rc = 0;
  std::cmatch matcher;
  std::string replacement2(replacement);
  std::string line2;
  line2.reserve(line.size() * 3);
  const char *start = line.c_str();
  while (std::regex_search(start, matcher, *searchExpression._regExpr)) {
    line2 += matcher.prefix();
    if (patternBackreference == nullptr) {
      line2 += replacement2;
    } else {
      std::cmatch matcher2;
      const char *start2 = replacement;
      while (std::regex_search(start2, matcher2, *patternBackreference)) {
        if (matcher2.size() != 2) {
          throw InternalError("missing group in patternBackreference");
        }
        line2 += matcher2.prefix();
        size_t groupNo = ::atol(matcher2.str(1).c_str());
        if (groupNo < matcher.size()) {
          line2 += matcher.str(groupNo);
        } else {
          line2 += matcher2.str();
        }
        start2 += matcher2.prefix().length() + matcher2.length();
      }
      line2 += start2;
    }
    start += matcher.position() + matcher.length();
    rc++;
    if (start[0] == '\0' || (count != -1 && --count <= 0)) {
      break;
    }
  }
  if (rc > 0) {
    line = line2 + start;
  }
  return rc;
}
bool LineList::search(const SearchExpression &searchExpression,
    SearchResult &result, bool setPosition) {
  bool rc = false;
  result._found = false;
  if (searchExpression._backwards) {
    rc = searchBackwardsRegExpr(*searchExpression._regExpr, result, setPosition,
        searchExpression._endOfLine, searchExpression._inline,
        searchExpression._flags.c_str());
  } else {
    rc = searchRegExpr(*searchExpression._regExpr, result, setPosition,
        searchExpression._beginOfLine, searchExpression._inline,
        searchExpression._flags.c_str());
  }
  if (rc) {
    _startLastHit = result._position;
    _lastHit = _lines[result._position._lineIndex].substr(
        result._position._columnIndex, result._length);
  }
  return rc;
}

/**
 * Searches the most right hit of the regular expression where the hit starts below a given column.
 */
bool LineList::searchBackwardsOneLine(size_t ixColumn, size_t ixLine,
    std::regex &regExpr, SearchResult &result) {
  const char *start = _lines[ixLine].c_str();
  const char *beginOfLine = start;
  std::cmatch matches;
  std::cmatch lastHit;
  bool rc = false;
  size_t columnHit = 0;
  while (std::regex_search(start, matches, regExpr)) {
    auto relativeColumn = matches.position(0);
    auto absColumn = start - beginOfLine + relativeColumn;
    auto endOfMatch = absColumn + matches.length();
    if (static_cast<size_t>(endOfMatch) > ixColumn) {
      break;
    }
    lastHit = matches;
    columnHit = absColumn;
// hits can overlap, don't skip length of pattern:
    start += relativeColumn + 1;
  }
  if (!lastHit.empty()) {
    rc = result._found = true;
    result._length = lastHit.length(0);
    result._position.set(ixLine, columnHit);
  }
  return rc;
}
bool LineList::searchBackwardsRegExpr(std::regex &regExpr, SearchResult &result,
    bool setPosition, bool endOfLine, bool inlineOnly, const char *flags) {
  bool rc = false;
  auto col = _position._columnIndex;
  auto line = min(_position._lineIndex, _lines.size() - 1);
  if (strchr(flags, '<')) {
    col = line = 0;
  } else if (strchr(flags, '>')) {
    line = _lines.size() - 1;
    col = END_OF_LINE;
  } else if (strchr(flags, '^')) {
    col = 0;
  } else if (strchr(flags, '$')) {
    col = END_OF_LINE;
  }
  if (_lines.size() > 0 && (line > 0 || col > 0)) {
    bool ready = false;
    std::cmatch matches;
// Search in the current line: may be only a part of the line:
    rc = searchBackwardsOneLine(col, line, regExpr, result);
    if (inlineOnly) {
      ready = true;
    }
    if (line-- == 0) {
      ready = true;
    }
    while (!rc && !ready && line >= 0) {
      rc = searchBackwardsOneLine(_lines[line].size(), line, regExpr, result);
      line--;
    }
    if (setPosition || strchr(flags, 'T') == nullptr) {
      if (rc) {
        this->setPosition(result._position._lineIndex,
            result._position._columnIndex + result._length - 1);
      }
    }
  }
  return rc;
}

bool LineList::searchRegExpr(std::regex &regExpr, SearchResult &result,
    bool setPosition, bool beginOfLine, bool inlineOnly, const char *flags) {
  auto line = _position._lineIndex;
  auto col = _position._columnIndex;
  if (strchr(flags, '<') != nullptr) {
    line = 0;
    col = 0;
  } else if (strchr(flags, '>') != nullptr) {
    line = _lines.size();
    col = 0;
  } else if (strchr(flags, '^') != nullptr) {
    col = 0;
  } else if (strchr(flags, '$') != nullptr) {
    col = END_OF_LINE;
  }
  bool rc = false;
  std::cmatch matches;
  if (line < _lines.size()) {
// Search in the first line: may be only a part of the line:
    if (col > 0) {
      if (!(col < _lines[line].size() && !beginOfLine)) {
        line++;
      } else {
        const char *start = _lines[line].c_str() + col;
        if (std::regex_search(start, matches, regExpr)) {
          rc = true;
          result._position._columnIndex = col + matches.position(0) - 1;
          result._length = matches.length(0);
        } else {
          line++;
        }
        if (inlineOnly) {
          if (!rc && setPosition) {
            this->setPosition(line - 1, END_OF_LINE);
          }
        }
      }
    }
    while (!rc && line < _lines.size()) {
      if (std::regex_search(_lines[line].c_str(), matches, regExpr)) {
        rc = true;
        result._position._lineIndex = line;
        result._position._columnIndex = matches.position(0);
        result._length = matches.length(0);
        break;
      }
      if (inlineOnly) {
        break;
      }
      line++;
    }
    if (setPosition && strchr(flags, 'T') == nullptr) {
      if (rc) {
        this->setPosition(result._position._lineIndex,
            result._position._columnIndex + result._length);
      }
    }
  }
  return rc;
}

bool LineList::readFromFile(const char *filename, bool stripNewline) {
  _currentFilename = filename;
  _name = basename(filename);
  LineReader reader(nullptr, _logger, stripNewline);
  reader.openFile(filename);

  LineAgent lineAgent(&_logger);
  lineAgent.openFile(filename);
  auto lineCount = lineAgent.estimateLineCount();
  if (lineCount > 0) {
    _lines.reserve(lineCount * 3 / 2);
  }
  bool rc = true;
  size_t length = 0;
  const char *line;
  while ((line = lineAgent.nextLine(length)) != nullptr) {
    if (lineAgent.hasBinaryData()) {
      rc = false;
      break;
    }
    if (stripNewline && length > 0 && line[length - 1] == '\n') {
      length--;
    }
    _lines.push_back(std::string(line, length));
  }
  return rc;
}

BufferPosition& LineList::setMark(const BufferPosition &position) {
  _mark = position;
  size_t length = 0;
  if (_mark._lineIndex >= _lines.size()) {
    _mark._lineIndex = _lines.size();
    _mark._columnIndex = 0;
  } else if (_mark._columnIndex > (length = _lines[_mark._lineIndex].size())) {
    _mark._columnIndex = length;
  }
  return _mark;
}

BufferPosition& LineList::setPosition(int lineIndex, int columnIndex) {
  if (lineIndex >= 0) {
    _position._lineIndex = min(_lines.size(), lineIndex);
  }
  if (_position._lineIndex >= _lines.size()) {
    _position._columnIndex = 0;
  } else if (columnIndex >= 0) {
    _position._columnIndex = min(_lines[_position._lineIndex].size(),
        columnIndex);
  }
  return _position;
}

BufferPosition& LineList::setPosition(const BufferPosition &position) {
  setPosition(position._lineIndex, position._columnIndex);
  return _position;
}

bool LineList::writeToFile(const char *filename, bool force, bool append) {
  bool rc = false;
  if (strcmp(filename, _currentFilename.c_str()) != 0 || force || _hasChanged) {
    _currentFilename = filename;
    FILE *fp = fopen(filename, append ? "a" : "w");
    if (fp != nullptr) {
      for (auto line : _lines) {
        fputs(line.c_str(), fp);
        if (line.back() != '\n') {
          fputc('\n', fp);
        }
      }
      rc = true;
      fclose(fp);
    }
  }
  return rc;
}

} /* namespace cppknife */

