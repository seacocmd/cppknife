/*
 * JsonReader.cpp
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {
const double NodeJson::UNDEF_DOUBLE = 1E100;

JsonFormatError::JsonFormatError(const std::string &message,
    TokenInfo &tokenInfo) :
    InternalError(tokenInfo.formatError(message)) {
}

NodeJson::NodeJson(JsonNodeType type) :
    _type(type) {
}
NodeJson::~NodeJson() {
}
NodeJson& NodeJson::operator[](size_t index) {
  throw InternalError(
      formatCString("node with type %s has no index access",
          typeToString(_type)));
}

const char* NodeJson::typeToString(JsonNodeType type) {
  const char *rc = nullptr;
  switch (type) {
  case JNT_UNDEFINED:
    rc = "<undefined>";
    break;
  case JNT_VALUE:
    rc = "Json-Value";
    break;
  case JNT_ARRAY:
    rc = "Json-Array";
    break;
  case JNT_MAP:
    rc = "Json-Map";
    break;
  }
  return rc;
}
ArrayJson& NodeJson::array() {
  throw InternalError(
      formatCString("node with type %s is not an array", typeToString(_type)));
}

bool NodeJson::asBool() const {
  throw InternalError(
      formatCString("node with type %s has no bool value",
          typeToString(_type)));
}
double NodeJson::asDouble(double defaultValue) const {
  throw InternalError(
      formatCString("node with type %s has no double value",
          typeToString(_type)));
}
int NodeJson::asInt(int defaultValue) const {
  throw InternalError(
      formatCString("node with type %s has no int value", typeToString(_type)));
}
const char* NodeJson::asString() const {
  throw InternalError(
      formatCString("node with type %s has no string value",
          typeToString(_type)));
}

NodeJson& NodeJson::map(const char *label) {
  throw InternalError(
      formatCString("node with type %s is not a map", typeToString(_type)));
}

ValueJson::ValueJson(const char *value) :
    NodeJson(JNT_VALUE), _value(value) {
}
ValueJson::~ValueJson() {
}
bool ValueJson::asBool() const {
  bool rc = _value[0] == 't';
  return rc;
}
double ValueJson::asDouble(double defaultValue) const {
  char *endPtr;
  double rc = ::strtod(_value, &endPtr);
  if (endPtr[0] != '\0') {
    rc = defaultValue;
  }
  return rc;
}
int ValueJson::asInt(int defaultValue) const {
  char *endPtr;
  int rc = (int) ::strtol(_value, &endPtr, 10);
  if (endPtr[0] != '\0') {
    rc = defaultValue;
  }
  return rc;
}
const char* ValueJson::asString() const {
  return _value;
}

ArrayJson::ArrayJson() :
    NodeJson(JNT_ARRAY), _array() {
}
ArrayJson::~ArrayJson() {
  for (auto item : _array) {
    delete item;
  }
  _array.clear();
}

NodeJson& ArrayJson::operator [](size_t index) {
  if (index < 0 || index > _array.size()) {
    throw InternalError(
        formatCString("not entry at [%d]: [0..%d]", index, _array.size()));
  }
  return *_array[index];
}

MapJson::MapJson() :
    NodeJson(JNT_MAP), _map() {
}
MapJson::~MapJson() {
  for (auto pair : _map) {
    delete pair.second;
  }
  _map.clear();
}
void MapJson::add(const char *label, NodeJson *item) {
  _map[label] = item;
}

NodeJson& MapJson::map(const char *label) {
  auto it = _map.find(label);
  if (it == _map.end()) {
    throw InternalError(formatCString("label %s not found in map", label));
  }
  return *it->second;
}

TokenInfo::TokenInfo(ByteBuffer &byteBuffer) :
    _byteBuffer(byteBuffer), _stream(nullptr), _input(), _endOfInput(nullptr), _beginOfLine(
        nullptr), _cursor(nullptr), _length(0), _filename(), _lineNo(0), _string(
        nullptr) {
}
void TokenInfo::fetchNext() {
  _input.clear();
  if (_stream->fetch(_input)) {
    _lineNo++;
  }
  _beginOfLine = _cursor = _input.c_str();
  _endOfInput = _cursor + _input.size();
  _length = 0;
}
std::string TokenInfo::formatError(const std::string &message) {
  std::string rc;
  if (_filename.empty()) {
    rc = formatCString("%d (%d): %s", _lineNo, 1 + _cursor - _beginOfLine,
        message.c_str());
  } else {
    rc = formatCString("%s-%d (%d): %s", _filename.c_str(), _lineNo,
        1 + _cursor - _beginOfLine, message.c_str());
  }
  return rc;
}
void TokenInfo::skipNumber() {
  char *end = nullptr;
  ::strtod(_cursor, &end);
  _length = end - _cursor;
  store();
  _cursor += _length;
}
void TokenInfo::skipString() {
  char cc;
  const char *ptr = _cursor + 1;
  while (ptr < _endOfInput && (cc = *ptr++) != '"') {
    if (cc == '\\') {
      ptr++;
    }
  }
  if (cc != '"') {
    throw JsonFormatError("missing ending \"", *this);
  }
  _length = ptr - _cursor - 2;
  _cursor++;
  store();
  _cursor += _length + 1;
}

void TokenInfo::skipWhitespaces() {
  char cc;
  while (_cursor < _endOfInput
      && ((cc = *_cursor) == ' ' || cc == '\t' || cc == '\n' || cc == '\r')) {
    if (cc == '\n') {
      _lineNo++;
      _beginOfLine = _cursor + 1;
    }
    _cursor++;
  }
}
void TokenInfo::skipWord(size_t length) {
  _length = length;
  store();
  _cursor += _length;
}
void TokenInfo::store() {
  _string = _byteBuffer.allocate(_length);
  ::memcpy(_string, _cursor, _length);
}

void TokenInfo::setStream(LinesStream &stream) {
  _stream = &stream;
  _filename = stream.name();
  _lineNo = 0;
  _string = nullptr;
  fetchNext();
}

JsonReader::JsonReader(Logger &logger) :
    _storage(0x10000, nullptr), _root(nullptr), _logger(logger), _token(
        _storage) {
}

JsonReader::~JsonReader() {
  delete _root;
  _root = nullptr;
}

NodeJson& JsonReader::array() {
  if (_root == nullptr) {
    throw InternalError("root is not initialized");
  }
  if (_root->type() != JNT_ARRAY) {
    throw InternalError("root is not a map");
  }
  return _root->array();
}
NodeJson& JsonReader::map(const char *label) {
  if (_root == nullptr) {
    throw InternalError("root is not initialized");
  }
  if (_root->type() != JNT_MAP) {
    throw InternalError("root is not a map");
  }
  return _root->map(label);
}

JsonReader::TokenType JsonReader::next() {
  TokenType rc = TT_UNKNOWN;
  _token.skipWhitespaces();
  while (_token._cursor >= _token._endOfInput && !_token._stream->endOfInput()) {
    _token.fetchNext();
    _token.skipWhitespaces();
  }
  if (_token._cursor >= _token._endOfInput) {
    rc = TT_EOF;
  } else {
    _token._string = const_cast<char*>(_token._cursor);
    switch (*_token._cursor) {
    case '"':
      rc = TT_STRING;
      _token.skipString();
      break;
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      _token.skipNumber();
      rc = TT_NUMBER;
      break;
    case '{':
      rc = TT_BRACE_LEFT;
      _token._cursor++;
      break;
    case '}':
      rc = TT_BRACE_RIGHT;
      _token._cursor++;
      break;
    case '[':
      _token._cursor++;
      rc = TT_BRACKET_LEFT;
      break;
    case ']':
      rc = TT_BRACKET_RIGHT;
      _token._cursor++;
      break;
    case ':':
      rc = TT_COLON;
      _token._cursor++;
      break;
    case ',':
      rc = TT_COMMA;
      _token._cursor++;
      break;
    case 'n':
      if (strncmp(_token._cursor, "null", 4) == 0) {
        _token.skipWord(4);
        rc = TT_NULL;
      }
      break;
    case 't':
      if (strncmp(_token._cursor, "true", 4) == 0) {
        _token.skipWord(4);
        rc = TT_TRUE;
      }
      break;
    case 'f':
      if (strncmp(_token._cursor, "false", 5) == 0) {
        _token.skipWord(5);
        rc = TT_FALSE;
      }
      break;
    default:
      throw JsonFormatError(
          formatCString("unknown input: %.20s", _token._string), _token);
    }
  }
  return rc;
}

bool JsonReader::parse(LinesStream &stream) {
  bool rc = false;
  _token.setStream(stream);
  try {
    TokenType type = next();
    switch (type) {
    case TT_BRACE_LEFT:
      _root = parseMap();
      break;
    case TT_BRACKET_LEFT:
      _root = parseArray();
      break;
    default:
      throw JsonFormatError(
          formatCString("unexpected symbol: %.20s", _token._string), _token);
      break;
    }
    type = next();
    if (type != TT_EOF) {
      throw JsonFormatError(
          formatCString("unexpected trailing input: %.20s", _token._string),
          _token);
    }
    rc = true;
  } catch (const JsonFormatError &e) {
    _logger.say(LV_ERROR, e.message());
  }
  return rc;
}
ArrayJson* JsonReader::parseArray() {
  bool again = true;
  const size_t SIZE = 1024;
  NodeJson *list[SIZE];
  size_t nextIndex = 0;
  // deleted in the destructor of the parent (MapNode or ArrayNode)
  ArrayJson *rc = new ArrayJson();
  NodeJson *value = nullptr;
  int no = 0;
  while (again) {
    no++;
    TokenType type = next();
    if (type == TT_BRACKET_RIGHT) {
      break;
    }
    if (no > 1) {
      if (type != TT_COMMA) {
        throw JsonFormatError(
            formatCString("',' expected, not: %.20s", _token._string), _token);
      }
      type = next();
      if (type == TT_BRACKET_RIGHT) {
        break;
      }
    }
    switch (type) {
    case TT_BRACE_LEFT:
      value = parseMap();
      break;
    case TT_BRACKET_LEFT:
      value = parseArray();
      break;
    case TT_NUMBER:
    case TT_FALSE:
    case TT_TRUE:
    case TT_NULL:
      // deleted in the destructor of the parent (MapNode or ArrayNode).
      value = new ValueJson(_token._string);
      break;
    default:
      throw JsonFormatError(
          formatCString("value expected, not %.20s", _token._string), _token);
      break;
    }
    if (nextIndex >= SIZE) {
      rc->reserve(SIZE);
      for (size_t ix = 0; ix < SIZE; ix++) {
        rc->add(list[ix]);
      }
      nextIndex = 0;
    }
    list[nextIndex++] = value;
  }
  rc->reserve(nextIndex);
  for (size_t ix = 0; ix < nextIndex; ix++) {
    rc->add(list[ix]);
  }
  nextIndex = 0;
  return rc;
}
MapJson* JsonReader::parseMap() {
  bool again = true;
  // deleted in the destructor of the parent (MapNode or ArrayNode).
  MapJson *rc = new MapJson();
  const char *label;
  int no = 0;
  while (again) {
    no++;
    TokenType type = next();
    if (type == TT_BRACE_RIGHT) {
      break;
    }
    if (no > 1) {
      if (type != TT_COMMA) {
        throw JsonFormatError(
            formatCString("',' expected, not %.20s", _token._string), _token);
      }
      type = next();
      if (type == TT_BRACE_RIGHT) {
        break;
      }
    }
    if (type != TT_STRING) {
      throw JsonFormatError(
          formatCString("label expected, not %.20s", _token._string), _token);
    }
    label = _token._string;
    if (next() != TT_COLON) {
      throw JsonFormatError(
          formatCString("':' expected, not %.20s", _token._string), _token);
    }
    NodeJson *value = nullptr;
    type = next();
    switch (type) {
    case TT_BRACE_LEFT:
      value = parseMap();
      break;
    case TT_BRACKET_LEFT:
      value = parseArray();
      break;
    case TT_STRING:
    case TT_NUMBER:
    case TT_FALSE:
    case TT_TRUE:
    case TT_NULL:
      // deleted in the destructor of the parent (MapNode or ArrayNode).
      value = new ValueJson(_token._string);
      break;
    default:
      throw JsonFormatError(
          formatCString("value expected, not %.20s", _token._string), _token);
      break;
    }
    rc->add(label, value);
  }
  return rc;
}

} /* namespace cppknife */
