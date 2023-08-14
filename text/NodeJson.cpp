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
const char *NodeJson::_blanks =
    "                                                                                                                                    ";

bool NameAndType::nameInList(const char *attribute, NameAndType list[]) {
  bool rc = false;
  while (list->_attribute != nullptr) {
    if (strcmp(list->_attribute, attribute) == 0) {
      rc = true;
      break;
    }
    list++;
  }
  return rc;
}

JsonError::JsonError(const std::string &message) :
    InternalError(message) {
}

JsonError::JsonError(const char *message) :
    InternalError(message) {
}

JsonFormatError::JsonFormatError(const std::string &message,
    TokenInfo &tokenInfo) :
    JsonError(tokenInfo.formatError(message)) {
}

NodeJson::NodeJson(JsonNodeType type) :
    _type(type) {
}
NodeJson::~NodeJson() {
}
NodeJson& NodeJson::operator[](size_t index) {
  throw JsonError(
      formatCString("node with type %s has no index access",
          typeToString(_type)));
}

NodeJson& NodeJson::operator[](const char *attribute) {
  throw JsonError(
      formatCString("node with type %s has attribute %s", typeToString(_type),
          attribute));
}

void NodeJson::addBlanks2(int count, std::string &item) {
  while (count > _blankCount) {
    item += _blanks;
    count -= _blankCount;
  }
  if (count > 0) {
    item += std::string(_blanks, count);
  }
}
bool NodeJson::asBool() const {
  throw JsonError(
      formatCString("node with type %s has no bool value",
          typeToString(_type)));
}
double NodeJson::asDouble(double defaultValue) const {
  throw JsonError(
      formatCString("node with type %s has no double value",
          typeToString(_type)));
}
int NodeJson::asInt(int defaultValue) const {
  throw JsonError(
      formatCString("node with type %s has no int value", typeToString(_type)));
}
const char* NodeJson::asString() const {
  throw JsonError(
      formatCString("node with type %s has no string value",
          typeToString(_type)));
}
NodeJson* NodeJson::byAttribute(const char *attribute, bool throwException) {
  if (throwException) {
    throw JsonError(
        formatCString("node with type %s has no attribute %s",
            typeToString(_type), attribute));
  }
  return nullptr;
}
const NodeJson* NodeJson::byAttributeConst(const char *attribute,
    bool throwException) const {
  if (throwException) {
    throw JsonError(
        formatCString("node with type %s has no attribute %s",
            typeToString(_type), attribute));
  }
  return nullptr;
}
std::vector<NodeJson*>* NodeJson::array(bool throwException) {
  if (throwException) {
    throw JsonError(formatCString("node %s has no array", toString().c_str()));
  }
  return nullptr;
}
NodeJson* NodeJson::byIndex(int index, bool throwException) {
  if (throwException) {
    throw JsonError(
        formatCString("node with type %s has no index %d", typeToString(_type),
            index));
  }
  return nullptr;
}
const NodeJson* NodeJson::byIndexConst(int index, bool throwException) const {
  if (throwException) {
    throw JsonError(
        formatCString("node with type %s has no index %d", typeToString(_type),
            index));
  }
  return nullptr;
}
std::string NodeJson::checkStructure(NameAndType mandatory[],
    NameAndType optional[], bool mustComplete) const {
  auto rc = formatCString("not a MapJson: %s", typeToString(_type));
  return rc;
}

const char* NodeJson::dataTypeToString(JsonDataType dataType) {
  const char *rc = nullptr;
  switch (dataType) {
  case JDT_UNDEFINED:
    rc = "<undefined>";
    break;
  case JDT_ARRAY:
    rc = "array";
    break;
  case JDT_BOOL:
    rc = "bool";
    break;
  case JDT_FLOAT:
    rc = "float";
    break;
  case JDT_FLOAT_LIST:
    rc = "float list";
    break;
  case JDT_INT:
    rc = "int";
    break;
  case JDT_MAP:
    rc = "map";
    break;
  case JDT_STRING:
    rc = "string";
    break;
  case JDT_NULL:
    rc = "<null>";
    break;
  }
  return rc;
}

std::string NodeJson::decode(const NodeJson *tree, int indent) {
  std::string rc;
  size_t needed = indent == 0 ? 2 : 2 * indent;
  tree->addNeededBytes(needed, indent, 0);
  rc.reserve(needed);
  tree->addAsString(rc, indent, 0);
  if (rc.capacity() > needed) {
    printf("NodeJson::decode(): used/needed: %ld/%ld\n", rc.capacity(), needed);
  }
  return rc;
}

NodeJson* NodeJson::encode(const char *jsonString, std::string &error,
    Logger &logger) {
  error.clear();
  StringLinesStream stream(jsonString);
  JsonReader reader(logger);
  auto root = reader.parse(stream);
  if (root == nullptr) {
    error = reader.lastError();
  }
  return root;
}
const NodeJson* NodeJson::encodeFromFile(const char *filename,
    std::string &error, Logger &logger) {
  error.clear();
  FileLinesStream stream(filename, logger, false);
  JsonReader reader(logger);
  auto root = reader.parse(stream);
  if (root == nullptr) {
    error = reader.lastError();
  }
  return root;
}
bool NodeJson::isNull() const {
  return false;
}
bool NodeJson::hasAttribute(const char *attribute) const {
  return false;
}
std::map<std::string, NodeJson*>* NodeJson::map(bool throwException) {
  if (throwException) {
    throw JsonError(
        formatCString("the node %s has no map", toString().c_str()));
  }
  return nullptr;
}
const NodeJson* NodeJson::nodeByPath(const char *path[],
    JsonDataType expectedType, bool throwException) const {
  const NodeJson *rc = this;
  const char *reason = nullptr;
  std::string reason2;
  auto path2 = path;
  while (rc != nullptr && path2 != nullptr && *path2 != nullptr) {
    if (**path2 == '[') {
      if (rc->type() != JNT_ARRAY) {
        reason = "not an array";
        reason2 = rc->toString();
        rc = nullptr;
      } else {
        auto index = atol(path2[0] + 1);
        rc = rc->byIndexConst(index);
      }
    } else {
      if (_type != JNT_MAP) {
        reason = "not a map";
        reason2 = rc->toString();
        rc = nullptr;
      } else {
        rc = dynamic_cast<const MapJson*>(rc);
        if (!rc->hasAttribute(path2[0])) {
          reason = "missing attribute";
          reason2 = path2[0];
          rc = nullptr;
        } else {
          rc = rc->byAttributeConst(path2[0]);
        }
      }
    }
    path2++;
  }
  if (rc != nullptr && expectedType != JDT_UNDEFINED) {
    if (rc->dataType() != expectedType) {
      reason = "unexpected type";
      reason2 = dataTypeToString(rc->dataType());
      reason2 += " / ";
      reason2 += dataTypeToString(expectedType);
      rc = nullptr;
    }
  }
  if (rc == nullptr && throwException) {
    std::string message;
    auto path3 = path;
    while (path3 != nullptr && *path3 != nullptr) {
      if (!message.empty()) {
        message += '.';
      }
      message += *path3;
      path3++;
    }
    message += " ";
    message += reason;
    message += ": ";
    message += reason2;
    throw JsonError(message);
  }
  return rc;
}
const char* NodeJson::typeToString(JsonNodeType type) {
  const char *rc = nullptr;
  switch (type) {
  case JNT_UNDEFINED:
    rc = "<undefined>";
    break;
  case JNT_VALUE:
    rc = "JsonValue";
    break;
  case JNT_ARRAY:
    rc = "JsonArray";
    break;
  case JNT_MAP:
    rc = "JsonMap";
    break;
  }
  return rc;
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
    throw JsonError(
        formatCString("no entry at [%d]: [0..%d]", index, _array.size()));
  }
  return *_array[index];
}

void ArrayJson::addAsString(std::string &jsonString, int indent, int level,
    bool needsPrefix) const {
  if (indent == 0) {
    jsonString += '[';
  } else {
    jsonString += "[\n";
  }
  size_t no = 0;
  for (auto item : _array) {
    item->addAsString(jsonString, indent, level);
    if (++no < _array.size()) {
      jsonString += indent == 0 ? "," : ",\n";
    } else if (indent > 0) {
      jsonString += "\n";
    }
  }
  if (indent > 0) {
    addBlanks(indent * level, jsonString);
  }
  jsonString += ']';
}

size_t ArrayJson::addNeededBytes(size_t &needed, int indent, int level,
    bool needsPrefix) const {
  needed +=
      indent == 0 ?
          1 + sizeof _array + 1 : 2 + 2 * (sizeof _array) + indent * level + 1;
  for (auto item : _array) {
    item->addNeededBytes(needed, indent, level + 1);
  }
  return needed;
}

std::vector<NodeJson*>* ArrayJson::array(bool throwException) {
  return &_array;
}
NodeJson* ArrayJson::byIndex(int index, bool throwException) {
  NodeJson *rc = nullptr;
  if (index >= 0 && index < static_cast<int>(_array.size())) {
    rc = _array[index];
  } else if (throwException) {
    throw JsonError(
        formatCString("no entry at [%d]: [0..%d]", index, _array.size()));
  }
  return rc;
}

const NodeJson* ArrayJson::byIndexConst(int index, bool throwException) const {
  NodeJson *rc = nullptr;
  if (index >= 0 && index < static_cast<int>(_array.size())) {
    rc = _array[index];
  } else if (throwException) {
    throw JsonError(
        formatCString("no entry at [%d]: [0..%d]", index, _array.size()));
  }
  return rc;
}

JsonDataType ArrayJson::dataType() const {
  return JDT_ARRAY;
}

std::string ArrayJson::toString(int maxLength) const {
  std::string rc = "<array>";
  return rc;
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
NodeJson& MapJson::operator[](const char *attribute) {
  auto rc = byAttribute(attribute, true);
  return *rc;
}

void MapJson::add(const char *attribute, NodeJson *item,
    bool maskMetaCharacters) {
  if (hasAttribute(attribute)) {
    delete _map[attribute];
  }
  if (!maskMetaCharacters && escapeMetaCharactersCount(attribute) == 0) {
    _map[attribute] = item;
  } else {
    std::string key = attribute;
    escapeMetaCharacters(key);
    _map[key] = item;
  }
}

void MapJson::addAsString(std::string &jsonString, int indent, int level,
    bool needsPrefix) const {
  if (indent == 0) {
    jsonString += '{';
  } else {
    jsonString += "{\n";
  }
  size_t no = 0;
  for (auto item : _map) {
    if (indent > 0) {
      addBlanks(indent * level, jsonString);
    }
    jsonString += "\"";
    jsonString += item.first;
    if (indent == 0) {
      jsonString += "\":";
    } else {
      jsonString += "\": ";
    }
    item.second->addAsString(jsonString, indent, level + 1, false);
    if (++no < _map.size()) {
      jsonString += indent == 0 ? "," : ",\n";
    } else if (indent > 0) {
      jsonString += "\n";
    }
  }
  if (indent > 0) {
    addBlanks(indent * level, jsonString);
  }
  jsonString += '}';
}

size_t MapJson::addNeededBytes(size_t &needed, int indent, int level,
    bool needsPrefix) const {
  needed += indent == 0 ? 1 + 1 : 2 + indent * level + 1;
  for (auto item : _map) {
    needed += 3 + item.first.size() + indent * level;
    item.second->addNeededBytes(needed, indent, level + 1);
    needed += indent == 0 ? 1 : 2;
  }
  return needed;
}

NodeJson* MapJson::byAttribute(const char *attribute, bool throwException) {
  auto it = _map.find(attribute);
  auto rc = it == _map.end() ? nullptr : it->second;
  if (rc == nullptr && throwException) {
    throw JsonError(formatCString("unknown attribute: %s", attribute));
  }
  return rc;
}

const NodeJson* MapJson::byAttributeConst(const char *attribute,
    bool throwException) const {
  auto it = _map.find(attribute);
  auto rc = it == _map.end() ? nullptr : it->second;
  if (rc == nullptr && throwException) {
    throw JsonError(formatCString("unknown attribute: %s", attribute));
  }
  return rc;
}

std::string MapJson::checkStructure(NameAndType mandatory[],
    NameAndType optional[], bool mustBeComplete) const {
  std::string rc;
  for (int round = 0; round < 2; round++) {
    if (round == 1 && optional == nullptr) {
      break;
    }
    NameAndType *current = round == 0 ? &mandatory[0] : &optional[0];
    int ix = 0;
    while (current != nullptr && current->_attribute != nullptr) {
      const NodeJson *node = byAttributeConst(current->_attribute);
      if (node == nullptr) {
        if (round == 1) {
          current = &optional[++ix];
          continue;
        }
        rc += "\n";
        rc += formatCString("missing attribute %s", current->_attribute);
      } else {
        switch (current->_type) {
        default:
        case JDT_UNDEFINED:
          break;
        case JDT_ARRAY:
          if (node->type() != JNT_ARRAY) {
            rc += "\n";
            rc += formatCString("%s is not an array: %s", current->_attribute,
                node->toString().c_str());
          }
          break;
        case JDT_BOOL:
          if (node->dataType() != JDT_BOOL) {
            rc += "\n";
            rc += formatCString("%s is not a bool: %s", current->_attribute,
                node->toString().c_str());
          }
          break;
        case JDT_FLOAT:
          if (node->dataType() != JDT_INT && node->dataType() != JDT_FLOAT) {
            rc += "\n";
            rc += formatCString("%s is not a float: %s", current->_attribute,
                node->toString().c_str());
          }
          break;
        case JDT_FLOAT_LIST: {
          const char *value = node->asString();
          if (strspn(value, "01234567890 .,") != strlen(value)) {
            rc += "\n";
            rc += formatCString("%s is not a float list: %s",
                current->_attribute, node->toString().c_str());
          }
          break;
        }
        case JDT_INT:
          if (node->dataType() != JDT_INT) {
            rc += "\n";
            rc += formatCString("%s is not an int: %s", current->_attribute,
                node->toString().c_str());
          }
          break;
        case JDT_MAP:
          if (node->type() != JNT_MAP) {
            rc += "\n";
            rc += formatCString("%s is not a map: %s", current->_attribute,
                node->toString().c_str());
          }
          break;
        case JDT_STRING:
          if (node->dataType() != JDT_STRING) {
            rc += "\n";
            rc += formatCString("%s is not a string: %s", current->_attribute,
                node->toString().c_str());
          }
          break;
        }
      }
      current = round == 0 ? &mandatory[++ix] : &optional[++ix];
    }
  }
  if (mustBeComplete) {
    for (auto item : _map) {
      auto attribute = item.first.c_str();
      if (NameAndType::nameInList(attribute, mandatory)) {
        continue;
      }
      if (optional == nullptr) {
        continue;
      }
      if (NameAndType::nameInList(attribute, optional)) {
        continue;
      }
      rc += "\n";
      rc += formatCString("unknown attribute: %s", attribute);
    }
  }
  return rc;
}

JsonDataType MapJson::dataType() const {
  return JDT_MAP;
}

bool MapJson::erase(const char *attribute, bool deleteNode) {
  auto it = _map.find(attribute);
  bool rc = it != _map.end();
  if (rc) {
    if (deleteNode) {
      delete it->second;
    }
    _map.erase(attribute);
  }
  return rc;
}
bool MapJson::hasAttribute(const char *attribute) const {
  auto it = _map.find(attribute);
  bool rc = it != _map.end();
  return rc;
}
std::map<std::string, NodeJson*>* MapJson::map(bool throwException) {
  return &_map;
}
std::string MapJson::toString(int maxLength) const {
  std::string rc = "<map>";
  return rc;
}
ValueJson::ValueJson(const char *value) :
    NodeJson(JNT_VALUE), _value(value == nullptr ? "null" : value) {
  if (value == nullptr) {
    _dataType = JDT_NULL;
  } else if (isInt(value)) {
    _dataType = JDT_INT;
  } else if (isFloat(value)) {
    _dataType = JDT_FLOAT;
  } else if (strcmp(value, "null") == 0) {
    _dataType = JDT_UNDEFINED;
  } else if (strcmp(value, "true") == 0 || strcmp(value, "false") == 0) {
    _dataType = JDT_BOOL;
  } else {
    _dataType = JDT_STRING;
  }
}
ValueJson::~ValueJson() {
}
void ValueJson::addAsString(std::string &jsonString, int indent, int level,
    bool needsPrefix) const {
  if (indent > 0 && needsPrefix) {
    addBlanks(indent * level, jsonString);
  }
  switch (_dataType) {
  case JDT_FLOAT_LIST:
  case JDT_STRING: {
    jsonString += '"';
    jsonString += _value;
    jsonString += '"';
    break;
  }
  default:
    jsonString += _value;
    break;
  }
}
size_t ValueJson::addNeededBytes(size_t &needed, int indent, int level,
    bool needsPrefix) const {
  if (indent > 0 && needsPrefix) {
    needed += indent * level;
  }
  switch (_dataType) {
  case JDT_FLOAT_LIST:
  case JDT_STRING: {
    needed += 2 + _value.size();
    break;
  }
  default:
    needed += _value.size();
    break;
  }
  return needed;
}

bool ValueJson::asBool() const {
  bool rc = _value[0] == 't';
  return rc;
}
double ValueJson::asDouble(double defaultValue) const {
  char *endPtr;
  double rc = ::strtod(_value.c_str(), &endPtr);
  if (endPtr[0] != '\0') {
    rc = defaultValue;
  }
  return rc;
}
int ValueJson::asInt(int defaultValue) const {
  char *endPtr;
  int rc = (int) ::strtol(_value.c_str(), &endPtr, 10);
  if (endPtr[0] != '\0') {
    rc = defaultValue;
  }
  return rc;
}
const char* ValueJson::asString() const {
  return _value.c_str();
}

JsonDataType ValueJson::dataType() const {
  return _dataType;
}

bool ValueJson::isNull() const {
  return _dataType == JDT_NULL;
}

std::string ValueJson::toString(int maxLength) const {
  std::string rc = _value;
  if (_dataType != JDT_STRING) {

  }
  switch (_dataType) {
  case JDT_FLOAT_LIST:
  case JDT_STRING: {
    int length = _value.size();
    if (length < maxLength) {
      rc = _value;
    } else {
      rc = _value.substr(0, maxLength);
      rc + "..";
    }
    break;
  }
  default:
    rc = _value;
    break;
  }
  return rc;
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
    _storage(0x10000, nullptr), _logger(logger), _token(_storage), _error() {
}

JsonReader::~JsonReader() {
}

const std::string& JsonReader::lastError() const {
  return _error;
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
    case '-':
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

NodeJson* JsonReader::parse(LinesStream &stream) {
  NodeJson *root = nullptr;
  _token.setStream(stream);
  try {
    TokenType type = next();
    switch (type) {
    case TT_BRACE_LEFT:
      root = parseMap();
      break;
    case TT_BRACKET_LEFT:
      root = parseArray();
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
  } catch (const JsonFormatError &e) {
    _error = e.message();
    _logger.say(LV_ERROR, e.message());
    delete root;
    root = nullptr;
  } catch (const InternalError &e) {
    _error = e.message();
    _logger.say(LV_ERROR, e.message());
    delete root;
    root = nullptr;
  }
  return root;
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
    case TT_NULL:
      // deleted in the destructor of the parent (MapNode or ArrayNode).
      value = new ValueJson(nullptr);
      break;
    case TT_NUMBER:
    case TT_FALSE:
    case TT_TRUE:
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
  const char *attribute;
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
          formatCString("attribute expected, not %.20s", _token._string),
          _token);
    }
    attribute = _token._string;
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
      // deleted in the destructor of the parent (MapNode or ArrayNode).
      value = new ValueJson(_token._string);
      break;
    case TT_NULL:
      // deleted in the destructor of the parent (MapNode or ArrayNode).
      value = new ValueJson(nullptr);
      break;
    default:
      throw JsonFormatError(
          formatCString("value expected, not %.20s", _token._string), _token);
      break;
    }
    rc->add(attribute, value, false);
  }
  return rc;
}
} /* namespace cppknife */
