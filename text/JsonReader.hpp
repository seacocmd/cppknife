/*
 * JsonReader.hpp
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_JSONREADER_HPP_
#define TEXT_JSONREADER_HPP_
/// Implements a reader for files with a Json format.
namespace cppknife {

class TokenInfo;
/**
 * @brief This exception is used for Json format errors.
 */
class JsonFormatError: public InternalError {
public:
  JsonFormatError(const std::string &message, TokenInfo &tokenInfo);
};

enum JsonNodeType {
  JNT_UNDEFINED, JNT_VALUE, JNT_ARRAY, JNT_MAP
};
class ValueJson;
class ArrayJson;
class MapJson;
/**
 * @brief The base class for an node in the Json data tree.
 */
class NodeJson {
public:
  static const double UNDEF_DOUBLE;
  static const int UNDEF_INT = 0x80000000;
protected:
  JsonNodeType _type;
  NodeJson& operator [](size_t index);
public:
  NodeJson(JsonNodeType);
  virtual ~NodeJson();
public:
  virtual ArrayJson& array();
  virtual bool asBool() const;
  virtual double asDouble(double defaultValue = UNDEF_DOUBLE) const;
  virtual int asInt(int defaultValue = UNDEF_INT) const;
  virtual const char* asString() const;
  virtual NodeJson& map(const char *label);
  inline JsonNodeType type() const {
    return _type;
  }
public:
  static const char* typeToString(JsonNodeType type);
};

/**
 * @brief Stores a value in the Json data tree.
 */
class ValueJson: public NodeJson {
protected:
  const char *_value;
public:
  ValueJson(const char *value);
  ~ValueJson();
public:
  virtual bool asBool() const;
  virtual double asDouble(double defaultValue = UNDEF_DOUBLE) const;
  virtual int asInt(int defaultValue = UNDEF_INT) const;
  virtual const char* asString() const;
};
/**
 * @brief Stores an array in the Json data tree.
 */
class ArrayJson: public NodeJson {
protected:
  std::vector<NodeJson*> _array;
public:
  ArrayJson();
  virtual ~ArrayJson();
  NodeJson& operator [](size_t index);
public:
  inline void add(NodeJson *value) {
    _array.push_back(value);
  }
  virtual ArrayJson& array() {
    return *this;
  }
  inline void reserve(size_t addittionalSize) {
    _array.reserve(_array.size() + addittionalSize);
  }
};

/**
 * @brief Stores a map in the Json data tree.
 */
class MapJson: public NodeJson {
protected:
  std::map<const char*, NodeJson*, StringComparism> _map;
public:
  MapJson();
  virtual ~MapJson();
public:
  void add(const char *name, NodeJson *item);
  virtual NodeJson& map(const char *label);
};

/**
 * @brief Stores a syntactical element ("token") of the Json data stream.
 */
class TokenInfo {
public:
  ByteBuffer &_byteBuffer;
  LinesStream *_stream;
  std::string _input;
  /// Points to the EOS of _input.
  const char *_endOfInput;
  /// Points to the line start:
  const char *_beginOfLine;
  /// Points into the stream:
  const char *_cursor;
  /// Length of token pointed by _cursor:
  size_t _length;
  std::string _filename;
  int _lineNo;
  /// The current token as CString in ByteBuffer:
  char *_string;
public:
  TokenInfo(ByteBuffer &byteBuffer);
public:
  void fetchNext();
  std::string formatError(const std::string &message);
  void setStream(LinesStream &stream);
  void skipNumber();
  void skipString();
  void skipWhitespaces();
  void skipWord(size_t length);
  void store();
};
/**
 * @brief Transforms a Json text into a Json data tree.
 */
class JsonReader {
  enum TokenType {
    TT_UNKNOWN,
    TT_STRING,
    TT_NULL,
    TT_NUMBER,
    TT_COLON,
    TT_COMMA,
    TT_BRACKET_LEFT,
    TT_BRACKET_RIGHT,
    TT_BRACE_LEFT,
    TT_BRACE_RIGHT,
    TT_TRUE,
    TT_FALSE,
    TT_EOF
  };
protected:
  ByteBuffer _storage;
  NodeJson *_root;
  Logger &_logger;
  TokenInfo _token;
public:
  JsonReader(Logger &logger);
  virtual ~JsonReader();
public:
  TokenType next();
  bool parse(LinesStream &stream);
  ArrayJson* parseArray();
  MapJson* parseMap();
  NodeJson& array();
  NodeJson& map(const char *label);
};

} /* namespace cppknife */

#endif /* TEXT_JSONREADER_HPP_ */
