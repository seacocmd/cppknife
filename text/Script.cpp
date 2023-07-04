/*
 * Script.cpp
 *
 *  Created on: 09.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"
#include "../os/os.hpp"

namespace cppknife {
const std::vector<std::string> SearchParser::_operators = { "!=", "-eq", "-ge",
    "-gt", "-le", "-lt", "-ne", ":=", "<", "<=", "=", "==", ">", ">=", "~=" };
const std::regex SearchParser::_regexString("^([-+/\\\\*%$^=?\"',;.|]).*?\\1");
const std::regex SearchParser::_regexPattern(
    "^[srm]([-+/\\\\*%$^=?\"',;.|]).*?\\1[iLB<>$^]*");
const std::regex SearchParser::_regexBufferExpression(
    "^~[_A-Za-z]\\w*:\\d+([:-]\\d+)?");
const std::regex SearchParser::_regexBuffer("^~[_A-Za-z]\\w*");
const std::regex SearchParser::_regexVariable("^\\$\\([_A-Za-z]\\w*\\)");
const std::regex SearchParser::_regexVariable2("\\$\\([_A-Za-z]\\w*\\)");
const std::regex SearchParser::_regexParameterDefinition("^[a-z]\\w*=\\w+");
const std::regex SearchParser::_regexHereDocument("^<<\\S+");
const std::regex SearchParser::_regexFrom("^from");
const std::regex SearchParser::_regexFunctions(
    "^(buffer|math|os|string)\\.[a-z]\\w+");
const std::regex SearchParser::_regexAbsolutePosition("^(\\d+):(\\d+)");
const std::regex SearchParser::_regexOperator1("^[-+/:*%=<>~]");
const std::regex SearchParser::_regexOperator2(
    "^([!<>=~:]=|-[lg][te]|-eq|-ne)");
const std::regex SearchParser::_regexBackReference("\\$(\\d+)");
const std::regex SearchParser::_regexFunction(
    "^(buffer|math|os|string)\\.[a-z]\\w+");
const std::regex SearchParser::_regexAssignment("^[a-zA-Z_]\\w*\\s*[?:]?=");

const std::vector<std::string> SearchParser::_keywords = { "assert", "call",
    "copy", "delete", "else", "endif", "endscript", "endwhile", "exit", "if",
    "insert", "leave", "load", "log", "mark", "move", "range", "replace",
    "script", "select", "stop", "store", "while", };

ParameterSet::ParameterSet() :
    _parameters() {
}
ParameterSet::ParameterSet(ParameterInfo *parameter) :
    _parameters() {
  add(*parameter);
}
ParameterSet::ParameterSet(ParameterInfo *parameters[]) :
    _parameters() {
  for (auto ix = 0; parameters[ix] != nullptr; ix++) {
    add(*parameters[ix]);
  }
}
ParameterSet::~ParameterSet() {
  for (auto [name, item] : _parameters) {
    delete item;
  }
  _parameters.clear();
}
void ParameterSet::add(ParameterInfo &parameter) {
  _parameters[parameter._name] = &parameter;
}

int ParameterSet::asInt(const char *name, int defaultValue) const {
  auto parameter = find(name);
  int rc = defaultValue;
  if (parameter != nullptr) {
    rc = ::atoi(parameter->_value.c_str());
  }
  return rc;
}
ParameterInfo* ParameterSet::find(const char *name) const {
  std::string key(name);
  ParameterInfo *rc = nullptr;
  auto it = _parameters.find(name);
  if (it != _parameters.end()) {
    rc = it->second;
  }
  return rc;
}
std::string ParameterSet::populate(const char *definition, std::string &key) {
  std::string rc;
  auto parts = splitCString(definition, "=", 2);
  auto parameter = find(parts[0].c_str());
  if (parameter == nullptr) {
    rc = formatCString("unknown parameter: %s", parts[0].c_str());
  } else {
    key = parts[0];
    parameter->_value = parts[1];
    switch (parameter->_dataType) {
    case ParameterInfo::DT_INT:
      if (!isInt(parts[1].c_str())) {
        rc = formatCString(
            "wrong data type for parameter %s (int expected): %s",
            parts[0].c_str(), parts[1].c_str());
      }
      break;
    default:
      break;
    }
  }
  return rc;
}

SearchParser::SearchParser(Logger &logger) :
    Parser(logger) {
  Parser::_keywords = _keywords;
  _reOperator1 = SearchParser::_regexOperator1;
  _reOperator2 = SearchParser::_regexOperator2;
  addSpecialTokenRegex(_regexBufferExpression);
  addSpecialTokenRegex(_regexBuffer);
  addSpecialTokenRegex(_regexVariable);
  addSpecialTokenRegex(_regexString);
  addSpecialTokenRegex(_regexPattern);
  addSpecialTokenRegex(_regexParameterDefinition);
  addSpecialTokenRegex(_regexFunction);
}
SearchParser::~SearchParser() {
}

LineBuffer* SearchParser::asBufferName(SearchEngine &engine, bool mustExist) {
  auto name = tokenAsString();
  auto rc = engine.getBuffer(name.c_str());
  if (rc == nullptr) {
    if (mustExist) {
      throw ParserError(formatCString("unknown buffer %s", name.c_str()),
          *this);
    }
    rc = engine.createBuffer(name.c_str());
  }
  return rc;
}
void SearchParser::asPosition(const std::string &value,
    BufferPosition &position) const {
  std::smatch matches;
  if (!std::regex_match(value, matches, _regexAbsolutePosition)) {
    throw ParserError(
        formatCString("Absolute position expected not: %s", value.c_str()),
        *this);
  }
  position._lineIndex = max(1, ::atol(matches[1].str().c_str())) - 1;
  position._columnIndex = max(1, ::atol(matches[2].str().c_str())) - 1;
}

void SearchParser::asPattern(SearchExpression &expression) {
  const std::string &token = _token._string;
  auto isRegExpr = true;
  const char *flags = strchr(token.c_str() + 2, token[1]) + 1;
  std::string pattern = token.substr(2, flags - token.c_str() - 3);
  switch (token[0]) {
  case 'm':
    pattern = globToRegularExpression(pattern.c_str(), pattern.size());
    break;
  case 's':
    pattern = stringToRegularExpression(pattern.c_str(), pattern.size());
    break;
  default:
    break;
  }
  expression.set(pattern.c_str(), isRegExpr, flags);
}

std::string SearchParser::nameOfBuffer(const std::string &name) {
  auto rc = name;
  if (rc[0] == '~') {
    rc.erase(0, 1);
  }
  return rc;
}
LineBuffer* SearchParser::parseBuffer(bool testOnly, SearchEngine *engine,
    bool mustBePresent, bool bufferMustExist) {
  LineBuffer *rc = nullptr;
  auto type = parseSpecial(&SearchParser::_regexBuffer, 1);
  if (type == 1) {
    if (!testOnly) {
      rc = asBufferName(*engine, bufferMustExist);
    }
  } else {
    if (mustBePresent && !testOnly) {
      throw ParserError(std::string("missing buffer name"), *this);
    } else {
      rc = engine->getBuffer();
    }
  }
  return rc;
}

int SearchParser::parseInt(bool testOnly, const char *name, bool mustExist,
    int defaultValue) {
  int rc = defaultValue;
  auto type = parseSpecial(&Parser::_regexInt, 1);
  if (type == 1) {
    rc = ::atoi(tokenAsCString());
  } else if (mustExist) {
    throw ParserError(formatCString("missing %s (a integer number).", name),
        *this);
  }
  return rc;
}
bool SearchParser::parsePattern(bool testOnly, const char *name, bool mustExist,
    SearchExpression &searchExpression) {
  bool rc = false;
  auto type = parseSpecial(&_regexPattern, 1);
  if (type == 1) {
    rc = true;
    asPattern(searchExpression);
  } else if (mustExist) {
    throw ParserError(formatCString("missing %s (a search expression).", name),
        *this);
  }
  return rc;
}

bool SearchParser::parsePosition(bool testOnly, const char *name,
    bool mustExist, BufferPosition &position) {
  bool rc = false;
  auto type = parseSpecial(&SearchParser::_regexAbsolutePosition, 1);
  if (type == 1) {
    asPosition(_token._string, position);
    rc = true;
  }
  return rc;
}
std::string SearchParser::parseString(bool testOnly, const char *name,
    const char *defaultValue, bool needed) {
  std::string rc;
  auto type = parseSpecial(&_regexString, 1);
  if (type == 1) {
    rc = tokenAsInterpolatedString();
  } else if (defaultValue == nullptr) {
    if (needed) {
      throw ParserError(formatCString("missing %s (a string).", name), *this);
    }
  } else {
    rc = defaultValue;
  }
  return rc;
}

BlockStackEntry::BlockStackEntry() :
    _type(BT_UNDEF), _lineIndex(0) {
}

BlockStackEntry::BlockStackEntry(BlockType type, size_t lineIndex) :
    _type(type), _lineIndex(lineIndex) {
}

Script::Script(const char *name, SearchEngine &parent, Logger &logger) :
    LineList(100, &logger), _name(name), _engine(parent), _parser(logger), _indexNextStatement(
        0), _variables(), _buffers(), _ifData(), _singleEndData(), _openBlocks(), _currentBuffer(
        nullptr), _bufferStack(), _functionEngine(nullptr), _numericalContext(
        false), _alreadyTested(false) {
  _bufferStack.reserve(10);
  _openBlocks.reserve(10);
  _currentBuffer = parent.getBuffer();
  _functionEngine = new FunctionEngine(this, &_engine, _parser, _logger);
}

Script::~Script() {
  for (const auto& [key, value] : _ifData) {
    delete value;
  }
  _ifData.clear();
  _singleEndData.clear();
  while (_openBlocks.size() > 0) {
    delete _openBlocks.back();
    _openBlocks.pop_back();
  }
  delete _functionEngine;
  _functionEngine = nullptr;
  for (const auto& [key, buffer] : _buffers) {
    delete buffer;
  }
  _buffers.clear();
}
void Script::_check() {
  static int id = 0;
  if (++id == 18) {
    id += 0;
  }
  auto len = _variables.size();
  printf("%d: %d\n", ++id, len);
}
std::string Script::asBuffer() const {
// remove the preceding '~':
  auto rc = _parser.tokenAsString().substr(1);
  return rc;
}

std::string Script::asString() const {
// remove the string delimiters:
  auto token = _parser.token();
  std::string rc;
  if (token._string.size() < 2) {
    rc = token._string;
  } else {
    rc = token._string.substr(1, token._string.size() - 2);
  }
  return rc;
}

void Script::assignment(bool testOnly) {
  auto ptr = _parser.tokenAsCString();
  auto length = strcspn(ptr, " \t:=");
  std::string name(ptr, length);
  _numericalContext = ::strchr(ptr + length, ':');
  bool conditionalAssignment = ::strchr(ptr + length, '?');
  if (_numericalContext) {
    assignmentNumeric(name, testOnly);
  } else {
    interpolate(testOnly);
    bool append = false;
    std::string value;
    auto type = _parser.parseSpecial(&SearchParser::_regexFunction, 1);
    if (type == 1) {
      value = _functionEngine->asString(testOnly, name);
    } else {
      value.reserve(_parser.input()._unprocessed.size());
      TokenType type = TT_UNKNOWN;
      while (type != TT_EOF) {
        type = _parser.parseSpecial();
        if (type == TT_UNKNOWN) {
          type = _parser.parse();
        }
        switch ((int) type) {
        case TT_EOF:
          break;
        case TT_IDENTIFIER: {
          if (_parser.isWord("append")) {
            append = true;
          } else {
            ParserError(
                formatCString("unexpected data: %s", _parser.tokenAsCString()),
                _parser);
          }
          break;
        }
        case SearchParser::TT_BUFFER_EXPRESSION:
          value += valueOfBufferExpression();
          break;
        case SearchParser::TT_STRING2:
          value += _parser.tokenAsInterpolatedString();
          break;
        case TT_NUMBER:
          value += _parser.tokenAsString();
          break;
        default:
          throw ParserError(
              formatCString(
                  "wrong data in assignment: %s instead of string, 'append', variable",
                  Parser::typeName(type)), _parser);
        }
      }
    }
    bool exists = variableExists(name.c_str());
    if (conditionalAssignment && exists) {
      // do nothing
    } else if (exists && append) {
      _variables[name] += value;
    } else {
      setVariable(name, value);
    }
    if (!testOnly && _engine._trace != nullptr) {
      fprintf(_engine._trace, "  %s -> %.80s\n", name.c_str(),
          _variables[name].c_str());
    }
  }
}
void Script::assignmentNumeric(const std::string &name, bool testOnly) {
// note: ":=" is yet skipped
  interpolate(testOnly, nullptr, true);
  double value = 0;
  auto type = _parser.parseSpecial(&SearchParser::_regexFunction, 1);
  if (type == 1) {
    value = _functionEngine->asNumeric(testOnly, name);
  } else {
    char theOperator = '\0';
    while (true) {
      auto type = _parser.parse();
      if (type != TT_NUMBER) {
        throw ParserError(
            formatCString("number or function expected, not '%s'",
                _parser.tokenAsCString()), _parser);
      } else {
        double value2 = ::strtod(_parser.tokenAsCString(), nullptr);
        switch (theOperator) {
        case '+':
          value += value2;
          break;
        case '-':
          value -= value2;
          break;
        case '*':
          value *= value2;
          break;
        case ':':
          value /= value2;
          break;
        case '/':
          value = static_cast<int>(value) / static_cast<int>(value2);
          break;
        case '%':
          value = static_cast<int>(value) % static_cast<int>(value2);
          break;
        case '\0':
          value = value2;
          break;
        default:
          break;
        }
        type = _parser.parse();
        if (type == TT_EOF) {
          break;
        }
        if (type != TT_OPERATOR) {
          throw ParserError(
              formatCString("operator expected, not %s",
                  _parser.tokenAsCString()), _parser);
        } else if (_parser.tokenAsString().size() != 1) {
          throw ParserError(
              formatCString("numeric operator expected, not %s",
                  _parser.tokenAsCString()), _parser);
        }
        char op2 = _parser.tokenAsString()[0];
        if (theOperator != '\0' && precedence(op2) != precedence(theOperator)) {
          throw ParserError(
              formatCString("different precedences: %c / %c", op2, theOperator),
              _parser);
        }
        theOperator = op2;
      }
    }
  }
  if (double(int(value)) == value) {
    setVariable(name, formatCString("%.0f", value));
  } else {
    setVariable(name, formatCString("%f", value));
  }
  if (!testOnly && _engine._trace != nullptr) {
    fprintf(_engine._trace, "  %s -> %.80s\n", name.c_str(),
        _variables[name].c_str());
  }
}
void Script::assertStatement(bool testOnly) {
  TokenType type = TT_UNKNOWN;
  if (!testOnly) {
    bool again = true;
    const char *name = nullptr;
    do {
      type = _parser.parseSpecial(&SearchParser::_regexVariable,
          SearchParser::TT_VARIABLE, &SearchParser::_regexBuffer,
          SearchParser::TT_BUFFER);
      if (type == SearchParser::TT_VARIABLE) {
        variableAsString(_parser.tokenAsString(), false);
      } else if (type == SearchParser::TT_BUFFER) {
        if (getBuffer(name = _parser.tokenAsCString()) == nullptr) {
          throw ParserError(formatCString("missing buffer: %s", name), _parser);
        }
      } else {
        again = false;
      }
    } while (again);
    _parser.assertToken(TT_EOF);
  }
}

void Script::call(bool testOnly) {
  Script *script = nullptr;
  std::string scriptName;
  if (_parser.parseSpecial(&Parser::_regexId, TT_IDENTIFIER) == TT_IDENTIFIER) {
    scriptName = _parser.tokenAsString();
    if ((script = _engine.scriptByName(scriptName)) == nullptr) {
      throw ParserError(
          formatCString("unknown internal script: %s", scriptName.c_str()).c_str(),
          _parser);
    }

  } else {
    auto scriptFile = _parser.parseString(testOnly, "script name");
    script = _engine.scriptByName(scriptFile);
    if (script == nullptr) {
      if (!fileExists(scriptFile.c_str())) {
        throw ParserError(
            formatCString("file not found: %s", scriptFile.c_str()).c_str(),
            _parser);
      }
      scriptName = basename(scriptFile.c_str());
      // freed in descriptor of _engine.
      script = _engine.scriptByName(scriptName);
      if (script == nullptr) {
        script = new Script(scriptName.c_str(), _engine, _logger);
        script->setLines(readAsList(scriptFile.c_str(), &_logger));
      }
    }
  }
  auto parameters = _parser.parseBuffer(testOnly, &_engine, false, true);
  std::string define;
  std::vector<std::string> parts;
  while (!(define = _parser.parseString(testOnly, "parameter", nullptr, false)).empty()) {
    if (!testOnly) {
      parts = splitCString(define.c_str(), "=", 2);
      if (parts.size() != 2) {
        throw ParserError(
            formatCString("not a variable assignment: %s", define.c_str()),
            _parser);
      }
      script->setVariable(parts[0], parts[1]);
    }
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    int lineNo = 0;
    if (parameters != nullptr) {
      lineNo++;
      for (auto line : parameters->lines()) {
        auto parts = splitCString(line.c_str(), "=", 2);
        if (parts.size() != 2) {
          throw ParserError(
              formatCString("parameter definition in line %d misses a '=': %s",
                  lineNo, line.c_str()), _parser);
        }
        trimString(parts[0]);
        script->setVariable(parts[0], parts[1]);
      }
    }
    _engine.pushScript(script, _indexNextStatement);
    _engine.testAndRun();
    _engine.popScript();
  }
}

void Script::check() {
  if (!_alreadyTested) {
    while (_indexNextStatement < _lines.size()) {
      oneStatement(true);
    }
    if (!_openBlocks.empty()) {
      throw ParserError(
          formatCString("missing end of block starting in line %d",
              _openBlocks.back()->_lineIndex), _parser);
    }
    _alreadyTested = true;
  }
}

void Script::copy(bool testOnly) {
  std::vector<std::string> contents2;
  std::vector<std::string> &contents = contents2;
  LineBuffer *buffer = nullptr;
  auto fromMode = _parser.hasWaitingWord("from");
  if (!fromMode) {
    getText(testOnly, contents);
    buffer = _parser.parseBuffer(testOnly, &_engine, false, false);
  } else {
    auto source = _parser.parseBuffer(testOnly, &_engine, true, true);
    BufferPosition start(0, 1);
    BufferPosition end(END_OF_FILE, 0);
    if (_parser.hasWaitingWord("starting")) {
      _parser.parsePosition(testOnly, "start", true, start);
    }
    bool including = _parser.hasWaitingWord("including");
    if (including || _parser.hasWaitingWord("excluding")) {
      _parser.parsePosition(testOnly, "end", true, end);
    }
    bool hasTo = _parser.hasWaitingWord("to");
    if (hasTo) {
      buffer = _parser.parseBuffer(testOnly, &_engine, false, false);
    } else {
      buffer = _engine.getBuffer();
    }
    if (!testOnly) {
      source->copyRange(contents2, start, end, !including);
    }
  }
  auto append = _parser.hasWaitingWord("append");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    buffer->setLines(contents, append);
  }
}

void Script::deleteStatement(bool testOnly) {
// delete <start> <end> [buffer] [excluding-start] [including-end]
  BufferPosition start;
  BufferPosition end;
  _parser.parsePosition(testOnly, "start-position", true, start);
  _parser.parsePosition(testOnly, "end-position", true, end);
  bool excludeStart = false;
  bool includeEnd = false;
  const char *list[] = { "exclude-start", "include-end", nullptr };
  int index = 0;
  LineBuffer *buffer = _parser.parseBuffer(testOnly, &_engine, false, true);
  auto type = _parser.parse();
  while (type != TT_EOF && (index = _parser.indexOfWords(list)) >= 0) {
    if (index == -1) {
      throw ParserError(
          formatCString("option exclude-start or include-end expected not %s",
              _parser.tokenAsCString()), _parser);
    }
    if (index == 0) {
      excludeStart = true;
    } else {
      includeEnd = true;
    }
    type = _parser.parse();
  }
  if (!testOnly) {
    if (excludeStart) {
      start._columnIndex++;
    }
    if (includeEnd && end._columnIndex > 0) {
      end._columnIndex--;
    }
    buffer->deleteRange(start, end);
    if (_engine._trace != nullptr) {
      fprintf(_engine._trace, "  deleting %ld:%ld - %ld:%ld\n",
          start._lineIndex + 1, start._columnIndex + 1, end._lineIndex + 1,
          end._columnIndex + 1);
    }
  }
}

void Script::elseStatement(bool testOnly) {
  if (testOnly) {
    if (_openBlocks.back()->_type != BlockStackEntry::BT_IF_THEN) {
      throw ParserError(std::string("else without if"), _parser);
    }
    _openBlocks.back()->_type = BlockStackEntry::BT_IF_ELSE;
    _openBlocks.back()->_lineIndex = _parser.input()._lineNo;
  } else {
    _indexNextStatement = _singleEndData.find(_indexNextStatement)->second;
  }
}
void Script::endIf(bool testOnly) {
  if (testOnly) {
    auto item = _openBlocks.back();
    if (item->_type != BlockStackEntry::BT_IF_THEN
        && item->_type != BlockStackEntry::BT_IF_ELSE) {
      throw ParserError(std::string("endif without if"), _parser);
    }
    delete item;
    _openBlocks.pop_back();
  }
}

void Script::endScript(bool testOnly) {
  // nothing to do
}

void Script::endWhile(bool testOnly) {
  if (testOnly) {
    auto item = _openBlocks.back();
    if (item->_type != BlockStackEntry::BT_WHILE) {
      throw ParserError(std::string("endwhile without while"), _parser);
    }
    delete item;
    _openBlocks.pop_back();
  } else {
    _indexNextStatement = _singleEndData[_indexNextStatement];
  }
}

void Script::error(const char *message) {
  _logger.say(LV_ERROR, _parser.prepareError(message));
}

void Script::exitStatement(bool testOnly) {
// exit [<exitcode>] [global]
  auto exitCode = _parser.parseInt("message", 0, true);
  auto isGlobal = _parser.hasWaitingWord("global");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (isGlobal) {
      throw AbortException(exitCode);
    } else {
      // Stop execution:
      _indexNextStatement = _lines.size();
    }
  }
}

size_t Script::findEndOfBlock(int line, int idStatement, int idEnd1, int idEnd2,
    int &idFound) {
  size_t index = line;
  size_t rc = 0;
  int nesting = 1;
  while (index < _lines.size()) {
    _parser.setInput(_lines[index].c_str(), index + 1, _name.c_str());
    auto type = _parser.parse();
    if (type == TT_KEYWORD) {
      auto currentId = _parser.token()._keywordId;
      if (currentId == idStatement) {
        nesting++;
      } else if (currentId == idEnd2) {
        if (--nesting == 0) {
          idFound = idEnd2;
          rc = index;
          break;
        }
      } else if (nesting == 1 && currentId == idEnd1) {
        idFound = idEnd1;
        rc = index;
        break;
      }
    }
    index++;
  }
  if (rc == 0) {
    throw ParserError(
        formatCString("missing block end for statement %s in line %ld",
            SearchParser::_keywords[idStatement].c_str(), line), _parser);
  }
  return rc;
}

void Script::getText(bool testOnly, std::vector<std::string> &contents) {
  if (_parser.parseSpecial(&SearchParser::_regexHereDocument, 1) == 1) {
    auto marker = _parser.tokenAsString().substr(2);
    bool doInterpolate = marker[0] != '\'';
    if (!doInterpolate) {
      marker = marker.substr(1, marker.size() - 2);
    }
    bool found = false;
    size_t ix = _indexNextStatement;
    while (ix < _lines.size()) {
      auto line = _lines[ix++];
      if (line == marker) {
        found = true;
        _indexNextStatement = ix;
        break;
      }
      if (!testOnly) {
        if (doInterpolate) {
          interpolate(testOnly, &line);
        }
        contents.push_back(line);
      }
    }
    if (!found) {
      throw ParserError(formatCString("missing end marker %s", marker.c_str()),
          _parser);
    }
  } else {
    auto type = _parser.parseSpecial();
    switch (static_cast<int>(type)) {
    case SearchParser::TT_STRING2: {
      if (!testOnly) {
        contents = splitString(_parser.tokenAsInterpolatedString(),
            Parser::_regexNewline);
      }
      break;
    }
    case SearchParser::TT_BUFFER:
      if (!testOnly) {
        auto buffer = _parser.asBufferName(_engine);
        if (buffer == nullptr) {
          error(
              formatCString("unknown buffer: %s", _parser.tokenAsCString()).c_str());
        }
        if (buffer != nullptr) {
          contents = buffer->constLines();
        }
      }
      break;
    case SearchParser::TT_BUFFER_EXPRESSION:
    default:
      throw ParserError(
          formatCString(
              "unexpected data: '%s' Expected buffer, buffer-expression or string",
              _parser.tokenAsCString()), _parser);

    }
  }
}

bool Script::getCondition(bool testOnly) {
  bool rc = false;
  auto type = _parser.parseSpecial(&SearchParser::_regexPattern,
      SearchParser::TT_PATTERN);
  SearchExpression searchExpr;
  if (type != SearchParser::TT_PATTERN) {
    rc = getConditionAsBool(testOnly, false);
  } else {
    _parser.asPattern(searchExpr);
    auto buffer = getBuffer();
    type = _parser.parseSpecial(&SearchParser::_regexBuffer, 1);
    if (type == 1) {
      buffer = _parser.asBufferName(_engine);
    }
    if (testOnly) {
      _parser.assertToken(TT_EOF);
    } else {
      SearchResult searchResult;
      rc = searchBuffer(*buffer, searchExpr, searchResult, false);
    }
  }

  return rc;
}
bool Script::getConditionAsBool(bool testOnly, bool tokenYetFetched) {
  TokenType type = TT_UNKNOWN;
  if (tokenYetFetched) {
    type = _parser.token()._type;
  } else {
    type = _parser.parseSpecial(&Parser::_regexFloat, TT_NUMBER,
        &SearchParser::_regexString, TT_STRING);
  }
  bool rc = false;
  bool isNumber = false;
  SearchParser::Operators op = SearchParser::OP_UNKNOWN;
  double numeric1 = 0;
  std::string op1(_parser.tokenAsString());
  std::string op2;
  switch (type) {
  case TT_NUMBER:
    isNumber = true;
    numeric1 = std::strtod(op1.c_str(), nullptr);
    break;
  case SearchParser::TT_STRING2:
  case TT_STRING:
    op1 = _parser.tokenAsRawString();
    break;
  default:
    throw ParserError(
        formatCString(
            "unexpected data in condition: '%s' Expected: number or string or comparison of numbers/strings.",
            _parser.tokenAsCString()), _parser);
  }
  type = _parser.parseSpecial(&SearchParser::_regexOperator2, TT_OPERATOR,
      &SearchParser::_regexOperator1, TT_OPERATOR);
  if (type == TT_OPERATOR) {
    bool isNumber2 = false;
    auto operator2 = _parser.tokenAsString();
    op = static_cast<SearchParser::Operators>(1
        + _parser.tokenAsIndex(SearchParser::_operators));
    if (op == SearchParser::OP_SIMILAR) {
      rc = getConditionAsSimilar(testOnly, op1);
    } else {
      type = _parser.parseSpecial(&Parser::_regexInt, TT_NUMBER,
          &SearchParser::_regexString, TT_STRING);
      op2 = _parser.tokenAsString();
      switch (type) {
      case TT_NUMBER:
        isNumber2 = true;
        break;
      case TT_STRING:
        op2 = _parser.tokenAsRawString();
        break;
      default:
        throw ParserError(
            formatCString(
                "unexpected data in condition: '%s' Expected: number or string or comparison of numbers/strings.",
                _parser.tokenAsCString()), _parser);
      }
      int comparison = ::strcmp(op1.c_str(), op2.c_str());
      switch (op) {
      case SearchParser::OP_EQ:
      case SearchParser::OP_NE:
      case SearchParser::OP_LT:
      case SearchParser::OP_LE:
      case SearchParser::OP_GT:
      case SearchParser::OP_GE: {
        if (!isNumber) {
          if (!isNumber) {
            throw ParserError(
                formatCString("Operand 1 is not a number '%s' Operator: %s.",
                    op1.c_str(), operator2.c_str()), _parser);
          }
        }
        if (!isNumber2) {
          throw ParserError(
              formatCString("Operand 2 is not a number '%s'.", op2.c_str()),
              _parser);
        }
        auto numeric2 = std::strtod(op2.c_str(), nullptr);
        switch (op) {
        case SearchParser::OP_EQ:
          rc = numeric1 == numeric2;
          break;
        case SearchParser::OP_NE:
          rc = numeric1 != numeric2;
          break;
        case SearchParser::OP_LT:
          rc = numeric1 < numeric2;
          break;
        case SearchParser::OP_LE:
          rc = numeric1 <= numeric2;
          break;
        case SearchParser::OP_GT:
          rc = numeric1 > numeric2;
          break;
        case SearchParser::OP_GE:
          rc = numeric1 >= numeric2;
          break;
        default:
          break;
        }
        break;
      }
      case SearchParser::OP_EQS:
        rc = comparison == 0;
        break;
      case SearchParser::OP_GES:
        rc = comparison >= 0;
        break;
      case SearchParser::OP_GTS:
        rc = comparison > 0;
        break;
      case SearchParser::OP_LES:
        rc = comparison <= 0;
        break;
      case SearchParser::OP_LTS:
        rc = comparison < 0;
        break;
      case SearchParser::OP_NES:
        rc = comparison != 0;
        break;
      case SearchParser::OP_ASSIGN_NUMERIC:
      case SearchParser::OP_ASSIGN:
        throw ParserError(
            formatCString("unexpected operator '%s'.", operator2.c_str()),
            _parser);
      case SearchParser::OP_SIMILAR:
      default:
        break;
      }
    }
  } else if (type == TT_UNKNOWN) {
    if (isNumber) {
      rc = numeric1 != 0.0;
    } else {
      rc = !op1.empty();
    }
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  }
  return rc;
}

bool Script::getConditionAsSimilar(bool testOnly, const std::string &op1) {
  bool rc = false;
  SearchExpression searchExpr;
  _parser.parsePattern(testOnly, "condition", true, searchExpr);
  rc = searchExpr.search(op1.c_str());
  return rc;
}

std::string Script::hasParameter(ParameterSet &parameterSet) {
  std::string rc;
  auto type = _parser.parseSpecial(&SearchParser::_regexParameterDefinition, 1);
  if (type == 1) {
    auto error = parameterSet.populate(_parser.tokenAsCString(), rc);
    if (!error.empty()) {
      throw ParserError(error, _parser);
    }
  }
  return rc;
}

void Script::ifStatement(bool testOnly) {
// Always interpolate the input:

  interpolate(testOnly, nullptr, _numericalContext);
  bool condition = getCondition(testOnly);
// Deleted in destructor:
  auto item = new BlockStackEntry(BlockStackEntry::BT_IF_THEN,
      _parser.input()._lineNo);
  _openBlocks.push_back(item);
  if (testOnly) {
// Search for "else" and "endif"
    int idFound = 0;
    auto indexEnd = findEndOfBlock(_indexNextStatement, SearchParser::KW_IF,
        SearchParser::KW_ELSE, SearchParser::KW_ENDIF, idFound);
    size_t indexElse = 0;
    if (idFound == SearchParser::KW_ELSE) {
      indexElse = indexEnd;
      indexEnd = findEndOfBlock(indexElse + 1, SearchParser::KW_IF,
          SearchParser::KW_ENDIF, SearchParser::KW_ENDIF, idFound);
    }
    auto item = new IfData(indexElse == 0 ? 0 : indexElse + 1, indexEnd + 1);
    _ifData[_indexNextStatement] = item;
    if (indexElse != 0) {
      _singleEndData[indexElse + 1] = indexEnd + 1;
    }
  } else {
    auto data = _ifData.find(_indexNextStatement)->second;
    if (!condition) {
      _indexNextStatement =
          data->_elseIndex == 0 ? data->_endifIndex : data->_elseIndex;
    }
  }
}

void Script::interpolate(bool testOnly, std::string *line,
    bool numericContext) {
  if (line == nullptr) {
    line = &_parser.input()._unprocessed;
  }
  std::string safe(*line);
  std::smatch matches;
  while (std::regex_search(*line, matches, SearchParser::_regexVariable2)) {
    auto contents = variableAsString(matches.str(0), testOnly);
    if (numericContext && contents.empty()) {
      line->replace(matches.position(0), matches.length(), "0");
    } else {
      line->replace(matches.position(0), matches.length(), contents);
    }
  }
  if (!testOnly && _engine._trace != nullptr && safe != *line) {
    fprintf(_engine._trace, "  -> %s\n", line->c_str());
  }
}
LineBuffer* Script::createBuffer(const char *name) {
  if (name[0] == '~') {
    name++;
  }
// Deletion in destructor of Script or SearchEngine:
  LineBuffer *rc = new LineBuffer(_logger, _engine);
  rc->setName(name);
  if (name[0] == '_') {
    _engine._buffers[name] = rc;
  } else {
    _buffers[name] = rc;
  }
  return rc;
}
void Script::leave(bool testOnly) {
  auto type = _parser.parse();
  bool condition = true;
  int count = 1;
  if (type == TT_NUMBER) {
    count = atoi(_parser.tokenAsCString());
    type = _parser.parse();
  }
  if (_parser.isWord("if")) {
    condition = getCondition(testOnly);
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
    size_t index = _indexNextStatement;
    int nesting = count;
    while (index < _lines.size()) {
      _parser.setInput(_lines[index].c_str(), index + 1, _name.c_str());
      auto type = _parser.parse();
      if (type == TT_KEYWORD) {
        auto currentId = _parser.token()._keywordId;
        if (currentId == SearchParser::KW_ENDIF
            || currentId == SearchParser::KW_ENDWHILE) {
          if (--nesting == 0) {
            this->_singleEndData[_indexNextStatement] = index + 1;
            break;
          }
        }
      }
      index++;
    }
    if (nesting != 0) {
      throw ParserError(
          formatCString(
              "to few block ends (%ld / %ld) for statement 'leave' in line %ld",
              count - nesting, count), _parser);
    }
  } else {
    if (condition) {
      _indexNextStatement =
          this->_singleEndData.find(_indexNextStatement)->second;
    }
  }
}

void Script::load(bool testOnly) {
  auto type = _parser.parseSpecial();
  if (type != SearchParser::TT_BUFFER) {
    throw ParserError(
        formatCString("expected: buffer name instead of %s",
            _parser.tokenAsCString()), _parser);
  }
  auto bufferName = _parser.tokenAsString();
  auto buffer = _parser.asBufferName(_engine, false);
  if (buffer == nullptr) {
    buffer = createBuffer(bufferName.c_str());
  }
  type = _parser.parseSpecial();
  if (type != SearchParser::TT_STRING2) {
    throw ParserError(
        formatCString("expected a string (as filename) instead of %s",
            _parser.tokenAsCString()), _parser);
  }
  if (testOnly) {
    if (type == TT_UNKNOWN) {
      type = _parser.parse();
    }
    _parser.assertToken(TT_EOF);
  }
  if (!testOnly) {
    auto filename = asString();
    buffer->readFromFile(filename.c_str(), true);
    if (_engine._trace != nullptr) {
      fprintf(_engine._trace, "  %s: %ld lines read\n", filename.c_str(),
          buffer->lines().size());
    }
  }
}

void Script::mark(bool testOnly) {
// mark set exchange [<buffer>] || mark {save | restore] <variable>
  auto type = _parser.parse();
  bool search = false;
  enum {
    UNKNOWN, SET, EXCHANGE, SAVE, RESTORE
  } mode = UNKNOWN;
  if (_parser.isWord("set")) {
    mode = SET;
    type = _parser.parse();
    if (_parser.isWord("search")) {
      search = true;
    }
  } else if (_parser.isWord("exchange")) {
    mode = EXCHANGE;
  } else if (_parser.isWord("save")) {
    mode = SAVE;
  } else if (_parser.isWord("restore")) {
    mode = RESTORE;
  } else {
    throw ParserError(
        formatCString("unknown mode for 'mark': %s", _parser.tokenAsCString()),
        _parser);
  }
  LineBuffer *buffer = nullptr;
  type = _parser.parseSpecial(&SearchParser::_regexBuffer, 1);
  if (type == 1) {
    buffer = _parser.asBufferName(_engine);
  }
  if (buffer == nullptr) {
    buffer = getBuffer();
  }
  std::string variable;
  if (mode == SAVE || mode == RESTORE) {
    type = _parser.parseSpecial(&SearchParser::_regexVariable, 1);
    if (type == 1) {
      variable = _parser.tokenAsString();
    } else {
      throw ParserError(
          formatCString("variable expected, not %s", _parser.tokenAsCString()),
          _parser);
    }
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    BufferPosition position;
    switch (mode) {
    case SET:
      if (search) {
        buffer->startLastHit(position);
      } else {
        buffer->position(position);
      }
      buffer->setMark(position);
      break;
    case EXCHANGE: {
      BufferPosition mark;
      buffer->position(position);
      buffer->mark(mark);
      buffer->setPosition(mark);
      buffer->setMark(position);
      break;
    }
    case SAVE:
      buffer->mark(position);
      setVariable(variable,
          formatCString("%ld:%ld", position._lineIndex + 1,
              position._columnIndex + 1));
      break;
    case RESTORE: {
      auto value = variableAsString(variable);
      _parser.asPosition(value, position);
      buffer->setMark(position);
      break;
    }
    default:
      break;
    }
    if (_engine._trace != nullptr) {
      buffer->mark(position);
      fprintf(_engine._trace, "  -> %ld:%ld\n", position._lineIndex + 1,
          position._columnIndex + 1);
    }
  }
}

void Script::move(bool testOnly) {
// move { <search-expression> | <position> } [<buffer>]
  SearchExpression expression;
  LineBuffer *buffer = nullptr;
  int line = -1;
  int column = -1;
  bool hasColumn = false;
  bool lineIsRelative = false;
  bool columnIsRelative = false;
  bool isSearch = _parser.parsePattern(testOnly, "search", false, expression);
  if (!isSearch) {
    line = _parser.parseInt(testOnly, "lineNo", true, 0);
    lineIsRelative = _parser.tokenAsString()[0] == '+';
    if (_parser.firstChar() == ':') {
      _parser.skipFirstChar();
      column = _parser.parseInt(testOnly, "column", true, 0);
      columnIsRelative = _parser.tokenAsString()[0] == '+';
      hasColumn = true;
    }
  }
  buffer = _parser.parseBuffer(testOnly, &_engine, false, true);
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (buffer == nullptr) {
      buffer = _engine.getBuffer();
    }
    if (isSearch) {
      SearchResult result;
      buffer->search(expression, result, true);
    } else {
      BufferPosition current;
      buffer->position(current);
      if (lineIsRelative || line <= 0) {
        current._lineIndex += line;
      } else {
        // the absolute position in command is 1-based, transform to 0-based:
        current._lineIndex = max(0, column - 1);
      }
      if (hasColumn) {
        if (columnIsRelative || column <= 0) {
          current._columnIndex += column;
        } else {
          // the absolute position in command is 1-based, transform to 0-based:
          current._columnIndex = max(0, column - 1);
        }
      }
      buffer->setPosition(current);
    }
    if (_engine._trace != nullptr) {
      BufferPosition position;
      buffer->position(position);
      fprintf(_engine._trace, "  -> %ld:%0ld\n", position._lineIndex + 1,
          position._columnIndex + 1);
    }
  }
}

void Script::insert(bool testOnly) {
// insert [<buffer>] <position> <text>
  LineBuffer *buffer = nullptr;
  auto type = _parser.parseSpecial(&SearchParser::_regexBuffer, 1);
  if (type == 1) {
    buffer = _parser.asBufferName(_engine);
  }
  if (buffer == nullptr) {
    buffer = getBuffer();
  }
  type = _parser.parseSpecial(&SearchParser::_regexAbsolutePosition, 1);
  if (type != 1) {
    throw ParserError(
        formatCString("start position expected not %s",
            _parser.tokenAsCString()), _parser);
  }
  BufferPosition position;
  _parser.asPosition(_parser.tokenAsString(), position);
  std::vector<std::string> text;
  getText(testOnly, text);
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    buffer->insert(position, text);
    if (_engine._trace != nullptr) {
      auto last = text.size() - 1;
      if (last == 0) {
        fprintf(_engine._trace,
            "  inserted: %ld line(s) at %ld:%ld: %.80s...\n", text.size(),
            position._lineIndex + 1, position._columnIndex + 1,
            text[0].c_str());
      } else {
        fprintf(_engine._trace,
            "  inserted: %ld lines at %ld:%ld: [0]: %.40s... [%ld]: %s.40s\n",
            text.size(), position._lineIndex + 1, position._columnIndex + 1,
            text[0].c_str(), last, text[last].c_str());
      }
    }
  }
}

void Script::log(bool testOnly) {
  std::vector<std::string> contents2;
  std::vector<std::string> &contents = contents2;
  getText(testOnly, contents);
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    for (auto line : contents) {
      _logger.say(LV_INFO, line.c_str());
    }
  }
}

void Script::oneStatement(bool testOnly) {
  const std::string &line = _lines[_indexNextStatement];
  _indexNextStatement++;
  _parser.setInput(line.c_str(), _indexNextStatement, _name.c_str());
  if (!testOnly && _engine._trace != nullptr) {
    fprintf(_engine._trace, "%s-%03ld: %s\n", _name.c_str(),
        _indexNextStatement, line.c_str());
  }
  _numericalContext = false;
  if (_parser.parseSpecial(&SearchParser::_regexAssignment, 1) == 1) {
    assignment(testOnly);
  } else {
    TokenType type = _parser.parse();
    switch (type) {
    case TT_COMMENT:
      break;
    case TT_KEYWORD:
      if (_parser.input()._unprocessed[0] == '!') {
        _parser.input()._unprocessed.erase(0, 1);
        if (_parser.input()._unprocessed[0] == '!') {
          _parser.input()._unprocessed.erase(0, 1);
          _numericalContext = true;
        }
        interpolate(testOnly, nullptr, _numericalContext);
      }
      switch (_parser.token()._keywordId) {
      case SearchParser::KW_ASSERT:
        assertStatement(testOnly);
        break;
      case SearchParser::KW_CALL:
        call(testOnly);
        break;
      case SearchParser::KW_COPY:
        copy(testOnly);
        break;
      case SearchParser::KW_DELETE:
        deleteStatement(testOnly);
        break;
      case SearchParser::KW_ELSE:
        elseStatement(testOnly);
        break;
      case SearchParser::KW_ENDIF:
        endIf(testOnly);
        break;
      case SearchParser::KW_EXIT:
        exitStatement(testOnly);
        break;
      case SearchParser::KW_IF:
        ifStatement(testOnly);
        break;
      case SearchParser::KW_INSERT:
        insert(testOnly);
        break;
      case SearchParser::KW_WHILE:
        whileStatement(testOnly);
        break;
      case SearchParser::KW_ENDWHILE:
        endWhile(testOnly);
        break;
      case SearchParser::KW_LEAVE:
        leave(testOnly);
        break;
      case SearchParser::KW_LOAD:
        load(testOnly);
        break;
      case SearchParser::KW_LOG:
        log(testOnly);
        break;
      case SearchParser::KW_MARK:
        mark(testOnly);
        break;
      case SearchParser::KW_MOVE:
        move(testOnly);
        break;
      case SearchParser::KW_REPLACE:
        replace(testOnly);
        break;
      case SearchParser::KW_SCRIPT:
        script(testOnly);
        break;
      case SearchParser::KW_ENDSCRIPT:
        endScript(testOnly);
        break;
      case SearchParser::KW_SELECT:
        select(testOnly);
        break;
      case SearchParser::KW_STOP:
        stop(testOnly);
        break;
      case SearchParser::KW_STORE:
        store(testOnly);
        break;
      default:
        throw InternalError(
            formatCString("not implemented: %d (%s)", type,
                _parser.tokenAsCString()));
      }
      break;
    case TT_EOF:
      // empty line:
      break;
    default:
      throw InternalError(
          formatCString("unexpected (%d): %s", type, _parser.tokenAsCString()));
      break;
    }
  }
}
void Script::replace(bool testOnly) {
// replace <search-expr> <replacement> [<buffer>] [<start> [<end>]] [count=<count]  [if <filter-search-expr>]
// parent is a dummy:
  SearchExpression searchExpression;
  BufferPosition end(END_OF_LINE, END_OF_LINE);
  BufferPosition start;
  std::string line;
  std::string column;
  _parser.parsePattern(testOnly, "search", true, searchExpression);
  std::string replacement = _parser.parseString(testOnly, "replacement");
  LineBuffer *buffer = _parser.parseBuffer(testOnly, &_engine, false, true);
  if (_parser.parsePosition(testOnly, "start-position", false, start)) {
    _parser.parsePosition(testOnly, "end-position", false, end);
  }
  static ParameterSet parameters(
      new ParameterInfo("count", ParameterInfo::DT_INT));
  auto parameter = hasParameter(parameters);
  int count = 0x7fffffff;
  if (parameter == "count") {
    count = parameters.asInt("count");
  }
  SearchExpression filterExpression;
  bool hasFilter = _parser.hasWaitingWord("if");
  if (hasFilter) {
    _parser.parsePattern(testOnly, "filter", true, filterExpression);
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (buffer == nullptr) {
      buffer = _engine.getBuffer();
    }
    auto hits = buffer->replace(searchExpression, replacement.c_str(), count,
        &start, &end, hasFilter ? &filterExpression : nullptr,
        strchr(replacement.c_str(), '$') == nullptr ?
            nullptr : &SearchParser::_regexBackReference);
    if (_engine._trace != nullptr) {
      fprintf(_engine._trace, "  -> %d replacement(s)\n", hits);
    }
  }
}

void Script::setVariable(const std::string &name, const std::string &value) {
  std::string key(name[0] == '$' ? name : formatCString("$(%s)", name.c_str()));
  if (key[2] == '_') {
    if (key[3] == '_') {
      throw ParserError(
          formatCString("cannot modify readonly variable: %s", name.c_str()),
          _parser);
    }
    _engine.setGlobalVariable(key, value);
  } else {
    _variables[key] = value;
  }
}
bool Script::searchBuffer(LineBuffer &buffer, SearchExpression &searchExpr,
    SearchResult &searchResult, bool setPosition) {
  BufferPosition safe;
  buffer.position(safe);
  auto rc = buffer.search(searchExpr, searchResult, setPosition);
  if (!rc) {
    _engine._lastHit.clear();
  } else {
    auto ptr = buffer.lines()[searchResult._position._lineIndex].c_str()
        + searchResult._position._columnIndex;
    _engine._lastHit = std::string(ptr, searchResult._length);
  }
  if (_engine._trace != nullptr) {
    BufferPosition current;
    buffer.position(current);
    const char *pos = buffer.endOfData() ? " EOF" : "";
    fprintf(_engine._trace, "  %s: %ld-%ld -> %ld-%ld%s\n",
        buffer.name().c_str(), safe._lineIndex + 1, safe._columnIndex + 1,
        current._lineIndex + 1, current._columnIndex + 1, pos);
  }
  return rc;
}
int Script::precedence(char theOperator) {
  int rc = 0;
  switch (theOperator) {
  case '+':
  case '-':
    rc = 1;
    break;
  case '*':
  case '/':
  case ':':
  case '%':
    rc = 2;
    break;
  }
  return rc;
}
LineBuffer* Script::getBuffer(const char *name) {
  LineBuffer *rc = nullptr;
  if (name == nullptr) {
    rc = _currentBuffer == nullptr ? _engine.getBuffer() : _currentBuffer;
  } else {
    if (name[0] == '~') {
      name++;
    }
    if (name[0] == '_') {
// global buffer:
      rc = _engine.getBuffer(name);
    } else {
// local buffer
      auto it = _buffers.find(name);
      if (it != _buffers.end()) {
        rc = it->second;
      }
    }
  }
  return rc;
}
void Script::run() {
  _indexNextStatement = 0;
  while (_indexNextStatement < _lines.size()) {
    oneStatement(false);
  }
}

void Script::script(bool testOnly) {

  if (testOnly) {
    _parser.assertToken(TT_IDENTIFIER);
    auto name = _parser.tokenAsString();
    if (_engine.scriptByName(name.c_str()) != nullptr) {
      throw ParserError(formatCString("script %s already exists", name.c_str()),
          _parser);
    }
    /// Freed in the destructor of _engine:
    auto script = new Script(name.c_str(), _engine, _logger);
    _engine.addScript(script);
    auto start = _indexNextStatement - 1;
    do {
      script->lines().push_back(_lines[_indexNextStatement]);
      if (++_indexNextStatement >= _lines.size()) {
        throw ParserError(
            formatCString(
                "missing 'endscript' in script '%s' starting in line %d",
                name.c_str(), start), _parser);
      }
    } while (_lines[_indexNextStatement] != "endscript");
    _indexNextStatement++;
    _singleEndData[start + 1] = _indexNextStatement;
  } else {
    _indexNextStatement = _singleEndData.find(_indexNextStatement)->second;
  }
}
void Script::select(bool testOnly) {
// select [push] <buffer> | select pop
  if (_parser.hasWaitingWord("pop")) {
    if (!testOnly) {
      if (_bufferStack.size() < 1) {
        throw ParserError("pop on an empty stack", _parser);
      }
      _currentBuffer = _bufferStack.back();
      _bufferStack.pop_back();
    }
  } else {
    bool push = _parser.hasWaitingWord("push");
    auto buffer = _parser.parseBuffer(testOnly, &_engine, true, true);
    if (testOnly) {
      _parser.assertToken(TT_EOF);
    } else {
      if (push) {
        _bufferStack.push_back(_currentBuffer);
      }
      _currentBuffer = buffer;
    }
  }
}

void Script::stop(bool testOnly) {
// stop [<message>]
  auto message = _parser.parseString(testOnly, "message", "");
// Stops the script process:
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (!message.empty()) {
      _logger.error(message);
    }
    throw AbortException(-1);
  }
}

void Script::store(bool testOnly) {
  auto buffer = _parser.parseBuffer(testOnly, &_engine, false, true);
  auto filename = _parser.parseString(testOnly, "filename");
  bool append = _parser.hasWaitingWord("append");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    buffer->writeToFile(filename.c_str(), true, append);
  }
}

std::string Script::valueOfBufferExpression() {
  return "";
}

double Script::variableAsDouble(const std::string &name, bool quiet) {
  bool processed = false;
  BufferPosition position;
  double rc = 0;
  if (name.size() > 3 && name[2] == '_') {
    processed = true;
    auto buffer = getBuffer();
    if (name == "$(__line)") {
      rc = buffer->position(position)._lineIndex + 1;
    } else if (name == "$(__line0)") {
      rc = buffer->position(position)._lineIndex;
    } else if (name == "$(__column)") {
      rc = buffer->position(position)._columnIndex + 1;
    } else if (name == "$(__column0)") {
      rc = buffer->position(position)._columnIndex;
    } else if (name == "$(__lines)") {
      rc = buffer->lines().size();
    } else if (name == "$(__position)") {
    } else if (name == "$(__mark)") {
    } else if (name == "$(__start)") {
    } else if (name == "$(__end)") {
    } else if (name == "$(__length)") {
      rc = buffer->lastHit().size();
    } else if (name == "$(__buffer)") {
    } else if (name == "$(__file)") {
    } else if (name == "$(__date)") {
      rc = floor(nowAsDouble());
    } else if (name == "$(__time)") {
      auto now = nowAsDouble();
      rc = now - std::floor(now);
    } else if (name == "$(__hit)") {
      rc = strtod(buffer->lastHit().c_str(), nullptr);
    } else {
      processed = false;
    }
  }
  if (!processed) {
    auto value = variableAsString(name, quiet);
    rc = ::strtod(value.c_str(), nullptr);
  }
  return rc;
}

std::string Script::variableAsString(const std::string &name, bool quiet) {
  std::string rc;
  bool processed = false;
  BufferPosition position;
  if (name.size() > 3 && name[2] == '_') {
    processed = true;
    auto buffer = getBuffer();
    if (strncmp("$(_", name.c_str(), 3) == 0) {
      // global variable:
      if (name[3] != '_') {
        auto value = _engine.globalVariable(name.c_str());
        if (value == nullptr) {
          if (quiet) {
            value = "";
          } else {
            throw ParserError(
                formatCString("reading global variable before writing: %s",
                    name.c_str()), _parser);
          }
        }
        rc = value;
      } else if (name == "$(__line)") {
        rc = formatCString("%d", buffer->position(position)._lineIndex);
      } else if (name == "$(__lines)") {
        rc = formatCString("%d", buffer->lines().size());
      } else if (name == "$(__position)") {
        buffer->position(position);
        rc = formatCString("%ld:%ld", position._lineIndex + 1,
            position._columnIndex + 1);
      } else if (name == "$(__mark)") {
        buffer->mark(position);
        rc = formatCString("%ld:%ld", position._lineIndex + 1,
            position._columnIndex + 1);
      } else if (name == "$(__start)") {
        buffer->startLastHit(position);
        rc = formatCString("%ld:%ld", position._lineIndex + 1,
            position._columnIndex + 1);
      } else if (name == "$(__end)") {
        buffer->startLastHit(position);
        rc = formatCString("%ld:%ld", position._lineIndex + 1,
            position._columnIndex + 1 + buffer->lastHit().size());
      } else if (name == "$(__hit)") {
        rc = buffer->lastHit();
      } else if (name == "$(__buffer)") {
        rc = buffer->name();
      } else if (name == "$(__file)") {
        rc = buffer->filename();
      } else if (name == "$(__date)") {
        rc = formatDate();
      } else if (name == "$(__time)") {
        rc = formatTime(std::time(nullptr));
      } else {
        throw ParserError(
            formatCString("unknown intrinsic variable: %s", name.c_str()),
            _parser);
      }
    } else {
      processed = false;
    }
  }
  if (!processed) {
    auto it = _variables.find(name);
    if (it == _variables.end()) {
      if (!quiet) {
        error(
            formatCString("variable used before definition: %s", name.c_str()).c_str());
      }
    } else {
      rc = it->second;
    }
  }
  return rc;
}

bool Script::variableExists(const char *name) {
  bool rc = false;
  std::string name2(
      name[0] == '$' ? name : formatCString("$(%s)", name).c_str());
  if (name2[2] == '_') {
    rc = _engine.variableExists(name2.c_str());
  } else {
    rc = _variables.find(name2) != _variables.end();
  }
  return rc;
}
void Script::whileStatement(bool testOnly) {
  bool condition = getCondition(testOnly);
// Deleted in destructor:
  auto item = new BlockStackEntry(BlockStackEntry::BT_WHILE,
      _parser.input()._lineNo);
  _openBlocks.push_back(item);
  if (testOnly) {
// Search for "endwhile"
    int idFound = 0;
    auto indexEnd = findEndOfBlock(_indexNextStatement, SearchParser::KW_WHILE,
        SearchParser::KW_ENDWHILE, SearchParser::KW_ENDWHILE, idFound);
    _singleEndData[_indexNextStatement] = indexEnd + 1;
    _singleEndData[indexEnd + 1] = _indexNextStatement - 1;

  } else {
    if (!condition) {
      _indexNextStatement = _singleEndData.find(_indexNextStatement)->second;
    }
  }
}

} /* namespace cppknife */
