/*
 * Parser.cpp
 *
 *  Created on: 03.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include <ctype.h>
#include "text.hpp"

namespace cppknife {

const std::regex Parser::_regexNewline("\\r\\n|\\n\\r|[\\n\\r]");
const std::regex Parser::_regexWhitespaces("^\\s+");
const std::regex Parser::_regexId("^[_A-Za-z]\\w*");
const std::regex Parser::_regexInt("^[-+]?\\d+");
const std::regex Parser::_regexFloat("^[-+]?\\d+(\\.\\d+)?(E[+-]?\\d+)?");
const std::regex Parser::_regexOperator1("^[-+*/<>=]");
const std::regex Parser::_regexOperator2("^([<>=]=)");
const std::regex Parser::_regexOneLineComment("^\\s*#");
const std::regex Parser::_regexCommentStart("^/\\*");
const std::regex Parser::_regexCommentEnd("\\*/");
const std::regex Parser::_regexLineEnd("\\n");

Parser::Parser(Logger &logger) :
    _input(), _logger(logger), _token(), _nextToken(), _keywords(), _reSpecials(), _reWhitespaces(
        _regexWhitespaces), _reId(_regexId), _reInt(_regexInt), _reFloat(
        _regexFloat), _reOperator1(const_cast<std::regex&>(_regexOperator1)), _reOperator2(
        const_cast<std::regex&>(_regexOperator2)), _reOneLineComment(
        _regexOneLineComment), _reCommentStart(_regexCommentStart), _reCommentEnd(
        _regexCommentEnd) {
}

Parser::~Parser() {
}

void Parser::addSpecialTokenRegex(const std::regex &regExpr) {
  _reSpecials.push_back(&regExpr);
}

void Parser::assertCurrentToken(TokenType expectedType,
    const char *expectedStrings) {
  if (_token._type != expectedType) {
    throw ParserError(
        formatCString("unexpected token type: %s instead of %s: %s",
            typeName(_token._type), typeName(expectedType), tokenAsCString()),
        *this);
  }
  if (expectedStrings != nullptr) {
    auto token2 = formatCString(" %s ", _token._string.c_str());
    if (strstr(expectedStrings, token2.c_str()) == nullptr) {
      throw ParserError(
          formatCString("unexpected token: %s instead of%s",
              _token._string.c_str(), expectedStrings), *this);
    }
  }
}
void Parser::assertToken(TokenType expectedType, const char *expectedStrings) {
  parse();
  assertCurrentToken(expectedType, expectedStrings);
}

char Parser::firstChar() {
  std::smatch matches;
  if (std::regex_search(_input._unprocessed, matches, _reWhitespaces)) {
    _input._unprocessed.erase(0, matches[0].str().size());
  }
  return _input._unprocessed.empty() ? '\0' : _input._unprocessed[0];
}

int Parser::hasWaitingWord(const char *word1, const char *word2,
    const char *word3) {
  int rc = false;
  size_t length = strspn(_input._unprocessed.c_str(), " \t");
  if (length > 0) {
    _input._unprocessed.erase(0, length);
  }
  length = strlen(word1);
  if (strncmp(word1, _input._unprocessed.c_str(), length) == 0) {
    _input._unprocessed.erase(0, length);
    rc = 1;
  } else if (word2 != nullptr) {
    length = strlen(word2);
    if (strncmp(word2, _input._unprocessed.c_str(), length) == 0) {
      _input._unprocessed.erase(0, length);
      rc = 2;
    } else if (word3 != nullptr) {
      length = strlen(word3);
      if (strncmp(word3, _input._unprocessed.c_str(), length) == 0) {
        _input._unprocessed.erase(0, length);
        rc = 3;
      }
    }
  }
  return rc;
}
int Parser::indexOfWords(const char *words[]) {
  int rc = -1;
  int index = 0;
  const std::string &token = _token._string;
  while (words[index] != nullptr) {
    if (token == words[index]) {
      rc = index;
      break;
    }
    index++;
  }
  return rc;
}

bool Parser::isWord(const char *word) {
  bool rc = _token._string == word;
  return rc;
}

bool Parser::isOperator(const char *theOperator) {
  bool rc = _token._type == TT_OPERATOR
      && ::strcmp(_token._string.c_str(), theOperator) == 0;
  return rc;
}
TokenType Parser::lookAhead() {
  TokenType rc = TT_UNKNOWN;
  auto oldToken = _token;
  rc = parse();
  _nextToken = _token;
  _token = oldToken;
  return rc;
}

TokenType Parser::parse() {
  TokenType rc = TT_UNKNOWN;
  if (_nextToken._type != TT_UNKNOWN) {
    _token = _nextToken;
    _nextToken._string.clear();
    _nextToken._type = TT_UNKNOWN;
    rc = _token._type;
  } else {
    _token._keywordId = -1;
    _token._string.clear();
    std::smatch matches;
    const char *ptr = nullptr;
    if (std::regex_search(_input._unprocessed, matches, _reWhitespaces)) {
      _input._unprocessed.erase(0, matches[0].str().size());
    }
    char delimiter = 0;
    if ((delimiter = _input._unprocessed[0]) == '"' || delimiter == '\'') {
      size_t ix = 1;
      char cc = 0;
      while (ix < _input._unprocessed.size()
          && (cc = _input._unprocessed[ix]) != delimiter) {
        if (cc == '\\') {
          ix++;
        }
        ix++;
      }
      _token._string = _input._unprocessed.substr(0, ix + 1);
      _input._unprocessed.erase(0, ix + 1);
      rc = TT_STRING;
    } else if (std::regex_search(_input._unprocessed, matches,
        _reOneLineComment)) {
      _input._unprocessed.erase(0, matches[0].str().size());
      if ((ptr = strchr(_input._unprocessed.c_str(), '\n')) != nullptr) {
        _input._unprocessed.erase(0, ptr - _input._unprocessed.c_str());
      } else {
        _input._unprocessed.clear();
      }
      rc = TT_COMMENT;
    } else if (_input._unprocessed.empty()) {
      rc = TT_EOF;
    } else if (std::regex_search(_input._unprocessed, matches, _reFloat)
        || std::regex_match(_input._unprocessed, matches, _reInt)) {
      _token._string = matches[0];
      _input._unprocessed.erase(0, _token._string.size());
      rc = TT_NUMBER;
    } else if (std::regex_search(_input._unprocessed, matches, _reId)) {
      _token._string = matches[0];
      _input._unprocessed.erase(0, _token._string.size());
      rc = TT_IDENTIFIER;
      auto it = std::lower_bound(_keywords.begin(), _keywords.end(),
          _token._string);
      if (it != _keywords.end() && *it == _token._string) {
        _token._keywordId = std::distance(_keywords.begin(), it);
        rc = TT_KEYWORD;
      }
    } else if (std::regex_search(_input._unprocessed, matches, _reOperator2)) {
      _token._string = matches[0];
      _input._unprocessed.erase(0, _token._string.size());
      rc = TT_OPERATOR;
    } else if (std::regex_search(_input._unprocessed, matches, _reOperator1)) {
      _token._string = matches[0];
      _input._unprocessed.erase(0, _token._string.size());
      rc = TT_OPERATOR;
    }
  }
  _token._type = rc;
  return rc;
}

int Parser::parseInt(const char *name, int defaultValue, bool needed = false) {
  int rc = defaultValue;
  if (parseSpecial(&_regexInt, TT_NUMBER) == TT_NUMBER) {
    rc = ::atol(tokenAsCString());
  }
  return rc;
}
TokenType Parser::parseSpecial(const std::regex *regExpr1, int tokenType1,
    const std::regex *regExpr2, int tokenType2) {
  TokenType rc = TT_UNKNOWN;
  std::smatch matches;
  int specialId = TT_SPECIAL_1 - 1;
  _token._string.clear();
  if (std::regex_search(_input._unprocessed, matches, _reWhitespaces)) {
    _input._unprocessed.erase(0, matches[0].str().size());
  }
  if (regExpr1 != nullptr) {
    if (std::regex_search(_input._unprocessed, matches, *regExpr1)) {
      _token._string = matches[0];
      _token._keywordId = -1;
      rc = _token._type = static_cast<TokenType>(tokenType1);
      _input._unprocessed.erase(0, _token._string.size());
    }
    if (rc == TT_UNKNOWN && regExpr2 != nullptr
        && std::regex_search(_input._unprocessed, matches, *regExpr2)) {
      _token._string = matches[0];
      _token._keywordId = -1;
      rc = _token._type = static_cast<TokenType>(tokenType2);
      _input._unprocessed.erase(0, _token._string.size());
    }
  } else {
    for (auto regex : _reSpecials) {
      specialId++;
      if (std::regex_search(_input._unprocessed, matches, *regex)) {
        _token._string = matches[0];
        _token._keywordId = -1;
        rc = _token._type = static_cast<TokenType>(specialId);
        _input._unprocessed.erase(0, _token._string.size());
        break;
      }
    }
  }
  return rc;
}

const std::string Parser::prepareError(const char *message) const {
  std::string rc;
  if (!_input._filename.empty()) {
    rc = formatCString("%s-%d: %s\n--> %s", _input._filename.c_str(),
        _input._lineNo, _input._line.c_str(), message);
  } else if (_input._lineNo > 0) {
    rc = formatCString("%d: %s\n--> %s", _input._lineNo, _input._line.c_str(),
        message);
  } else {
    rc = message;
  }
  return rc;
}
void Parser::setInput(const char *input, int lineNo, const char *filename) {
  _input._filename = filename == nullptr ? "" : filename;
  _input._lineNo = lineNo;
  _input._unprocessed = _input._line = input;
  _token._string.clear();
}
void Parser::setKeywords(const std::vector<std::string> &keywords) {
  _keywords = keywords;
}

void Parser::skipFirstChar() {
  _input._unprocessed.erase(0, 1);
}

int Parser::tokenAsIndex(const std::vector<std::string> &words) const {
  int rc = 0;
  auto it = std::lower_bound(words.begin(), words.end(), _token._string);
  if (it != words.end() && *it == _token._string) {
    rc = std::distance(words.begin(), it);
  }
  return rc;
}

std::string Parser::tokenAsInterpolatedString() const {
// remove the delimiters:
  auto rc = _token._string.substr(1, _token._string.size() - 2);
  size_t ix = 0;
  while (ix < rc.size()) {
    if (rc[ix] != '\\') {
      ix++;
    } else {
      rc.erase(ix, 1);
      switch (rc[ix]) {
      case 't':
        rc[ix] = '\t';
        break;
      case 'n':
        rc[ix] = '\n';
        break;
      case 'r':
        rc[ix] = '\r';
        break;
      case 'v':
        rc[ix] = '\v';
        break;
      case '\0':
        // we have it deleted above, we must restore:
        rc += '\\';
        ix++;
        break;
      case 'x': {
        rc.erase(ix, 1);
        int hex = charToNibble(rc[ix]);
        if (hex < 0) {
          throw ParserError(
              formatCString(
                  "illegal hex digit in a \\x expression in the string: %c",
                  rc[ix]), *this);

        }
        int hex2 = charToNibble(rc[ix + 1]);
        if (hex2 >= 0) {
          hex = hex * 16 + hex2;
        }
        rc[ix] = hex;
        if (hex2 >= 0) {
          rc.erase(ix + 1, 1);
        }
        break;
      }
      default:
        break;
      }
      ix++;
    }
  }
  return rc;
}

std::string Parser::tokenAsRawString() const {
  return _token._string.substr(1, _token._string.size() - 2);
}

const std::string& Parser::tokenAsString() const {
  return _token._string;
}

const char* Parser::tokenAsCString() const {
  return _token._string.c_str();
}

const char* Parser::typeName(TokenType type) {
  const char *rc = nullptr;
  switch (type) {
  case TT_COMMENT:
    rc = "comment";
    break;
  case TT_EOF:
    rc = "end of input";
    break;
  case TT_IDENTIFIER:
    rc = "identifier";
    break;
  case TT_KEYWORD:
    rc = "keyword";
    break;
  case TT_NUMBER:
    rc = "number";
    break;
  case TT_OPERATOR:
    rc = "operator";
    break;
  default:
    rc = "?";
    break;
  }
  return rc;
}
} /* namespace cppknife */
