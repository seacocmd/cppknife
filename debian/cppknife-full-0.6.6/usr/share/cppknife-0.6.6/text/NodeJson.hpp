/*
 * JsonReader.hpp
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_NODEJSON_HPP_
#define TEXT_NODEJSON_HPP_
/// Implements a reader for files with a Json format.
namespace cppknife {

class TokenInfo;
/**
 * @brief This exception is used for any errors found in this module.
 */
class JsonError: public InternalError {
public:
  JsonError(const char *message);
  JsonError(const std::string &message);
};

/**
 * @brief This exception is used for Json format errors.
 */
class JsonFormatError: public JsonError {
public:
  JsonFormatError(const std::string &message, TokenInfo &tokenInfo);
};

enum JsonNodeType {
  JNT_UNDEFINED, JNT_VALUE, JNT_ARRAY, JNT_MAP
};
enum JsonDataType {
  JDT_UNDEFINED,
  JDT_ARRAY = 'a',
  JDT_BOOL = 'b',
  JDT_FLOAT = 'f',
  JDT_FLOAT_LIST = 'F',
  JDT_INT = 'i',
  JDT_MAP = 'm',
  JDT_STRING = 's',
  JDT_NULL = 'n',
};
class ValueJson;
class ArrayJson;
class MapJson;

class NameAndType {
public:
  const char *_attribute;
  JsonDataType _type;
public:
  static bool nameInList(const char *attribute, NameAndType list[]);
};

/**
 * @brief The base class for an node in the Json data tree.
 */
class NodeJson {
public:
  static const double UNDEF_DOUBLE;
  static const int UNDEF_INT = 0x80000000;
  static const char *_blanks;
  static const int _blankCount = 132;
protected:
  JsonNodeType _type;
public:
  NodeJson(JsonNodeType);
  virtual ~NodeJson();
  virtual NodeJson& operator [](size_t index);
  virtual NodeJson& operator [](const char *attribute);
public:
  /** Adds the instance to the Json string.
   * @param[in out] jsonString The text representation of the instance will be append here.
   * @param indent: 0: compact notation (no additional blanks and newlines).
   *  Otherwise: the number of blank for one indention level
   *  @param level The indention level of the instance.
   *  @param needsPrefix: <em>false</em>: the text representation of the instance should not indent.
   */
  virtual void addAsString(std::string &jsonString, int indent, int level,
      bool needsPrefix = true) const = 0;
  /** Calculates the length of the text representation of the instance and add it to a variable.
   * @param[in out] needed IN: the length of the Json string before handling the instance.
   *  OUT: the text representation of the instance has been added.
   * @param indent: 0: compact notation (no additional blanks and newlines).
   *  Otherwise: the number of blank for one indention level
   *  @param level The indention level of the instance.
   *  @param needsPrefix: <em>false</em>: the text representation of the instance should not indent.
   */
  virtual size_t addNeededBytes(size_t &needed, int indent, int level,
      bool needsPrefix = true) const = 0;
  /**
   * Returns the internal array of a <em>ArrayJson</em> instance or <em>nullptr</em>.
   * @
   */
  virtual std::vector<NodeJson*>* array(bool throwException = true);
  /**
   * Returns the bool value of the instance.
   * @param throwException <em>true</em>: throws an exception if null is returned.
   * @return <em>nullptr</em>: no array is available. Otherwise: the internal list
   *  of the <em>ArrayJson</em> instance.
   */
  virtual bool asBool() const;
  /**
   * Returns the double value of the instance.
   * Throws an exception if the instance is not a value node with double type.
   * @param defaultValue The result if the instance is not a double value.
   */
  virtual double asDouble(double defaultValue = UNDEF_DOUBLE) const;
  /**
   * Returns the int value of the instance.
   * Throws an exception if the instance is not a value node with int type.
   * @param defaultValue The result if the instance is not an int value.
   */
  virtual int asInt(int defaultValue = UNDEF_INT) const;
  /**
   * Returns the string value of the instance.
   * Throws an exception if the instance is not a value node with bool type.
   */
  virtual const char* asString() const;
  /**
   * Returns the value of a given attribute of the instance.
   * @param attribute The name of the attribute.
   * @param throwException <em>true</em>: returns an exception instead of returning <em>nullptr</em>
   * @return <em>nullptr</em>: the instance is not a map node or the attribute does not exist.
   *  Otherwise: the related node of the attribute.
   */
  virtual NodeJson* byAttribute(const char *attribute, bool throwException =
      false);
  /**
   * Returns the value of a given attribute of the instance as constant item.
   * @param attribute The name of the attribute.
   * @param throwException <em>true</em>: returns an exception instead of returning <em>nullptr</em>
   * @return <em>nullptr</em>: the instance is not a map node or the attribute does not exist.
   *  Otherwise: the related node of the attribute.
   */
  virtual const NodeJson* byAttributeConst(const char *attribute,
      bool throwException = false) const;
  /**
   * Returns the value of a given index of the instance.
   * @param index The index of the item to return.
   * @param throwException <em>true</em>: returns an exception instead of returning <em>nullptr</em>
   * @return <em>nullptr</em>: the instance is not an array node or the index is invalid.
   *  Otherwise: the the related node of the index.
   */
  virtual NodeJson* byIndex(int index, bool throwException = false);
  /**
   * Returns the value of a given index of the instance as constant item.
   * @param index The index of the item to return.
   * @param throwException <em>true</em>: returns an exception instead of returning <em>nullptr</em>
   * @return <em>nullptr</em>: the instance is not an array node or the index is invalid.
   *  Otherwise: the the related node of the index.
   */
  virtual const NodeJson* byIndexConst(int index,
      bool throwException = false) const;
  /***
   * Test whether the given Json tree is a map and has the given structure.
   * @param mandatory If one of that attributes does not exist an error occurrs.
   *  The attribute must have the defined type. The array must end with an entry with the <em>nullptr</em> as name.
   * @param optional: If one of that attributes does not exist an error occurrs.
   *  If the attribute exists it must have the defined type. The array must end with an entry with the <em>nullptr</em> as name.
   *  @param mustComplete <em>true</em>: only attributes listed in mandatory or in optional may be exist.
   * @return "": No error found. Otherwise: the error message(s).
   */
  virtual std::string checkStructure(NameAndType mandatory[],
      NameAndType optional[] = nullptr, bool mustComplete = false) const;
  /**
   * Returns the data type of the instance.
   */
  virtual JsonDataType dataType() const = 0;
  /**
   * Returns whether the Json value is null.
   */
  virtual bool isNull() const;
  /**
   * Tests whether the instance is a map and has the given attribute.
   * @param attribute: the attribute to search.
   * @param
   */
  virtual bool hasAttribute(const char *attribute) const;
  /**
   * Returns the internal map of a <em>NodeJson</em> instance of null for other nodes.
   * @param throwException <em>true</em>: throws an exception if null is returned.
   * @return <em>nullptr</em>: no map available. Otherwise: the map of the <em>NodeJson</em> instance.
   */
  virtual std::map<std::string, NodeJson*>* map(bool throwException = true);
  /**
   * Tests whether a given path exists in the Json tree.
   * @param path A list of attributes, ending with a <em>nullptr</em>.
   *  Each entry may be an attribute name or a index notation, e.g. "[1]" for the second index.
   *  Example: { "person", "list", "[3]", "name", nullptr }
   * @param expectedType If not JDT_UNDEFINED the found node must have the given type.
   *  If not <em>nullptr</em> is returned.
   *  @param throwException <em>true</em>: instead of returning <em>nullptr</em> an exception is thrown.
   */
  virtual const NodeJson* nodeByPath(const char *path[],
      JsonDataType expectedType = JDT_UNDEFINED,
      bool throwException = false) const;
  /**
   * Returns a textual representation of the instance.
   * @param maxLength The maximum length of the result.
   * @result: the textual representation of the instance. May be cut to the given maximum length.
   */
  virtual std::string toString(int maxLength = 40) const = 0;
  /**
   * Returns the node type of the instance.
   */
  inline JsonNodeType type() const {
    return _type;
  }
public:
  /**
   * Appends a given amount of blanks to a string.
   * @param count The number of blanks to add.
   * @param item The string to change.
   */
  inline static void addBlanks(int count, std::string &item) {
    if (count < _blankCount) {
      item += std::string(_blanks, count);
    } else {
      addBlanks2(count, item);
    }
  }
  /**
   * Appends a given amount of blanks to a string. This method can operate of each count.
   * @param count The number of blanks to add.
   * @param item The string to change.
   */
  static void addBlanks2(int count, std::string &item);
  /**
   * Converts a Json tree into a string.
   * @param tree The Json tree to convert.
   * @param indent 0: The result is a compact string.
   *  Otherwise: the result is a pretty printed string with that indent step.
   */
  static std::string decode(const NodeJson *tree, int indent = 0);
  /**
   * Converts a string into a Json tree.
   * @param jsonString The textual representation of the Json tree.
   * @param[out] error "": no error occurred. Otherwise: the error message.
   * @logger The logger.
   * @return <em>nullptr</em>: an error has occurred. Otherwise: the Json tree.
   */
  static NodeJson* encode(const char *jsonString, std::string &error,
      Logger &logger);
  /**
   * Converts a Json formatted file into a Json tree.
   * @param filename The file containing the Json formatted data.
   * @param[out] error "": no error occurred. Otherwise: the error message.
   * @logger The logger.
   * @return <em>nullptr</em>: an error has occurred. Otherwise: the Json tree.
   */
  static const NodeJson* encodeFromFile(const char *filename,
      std::string &error, Logger &logger);
  /**
   * Returns the text representation of a data type.
   */
  static const char* dataTypeToString(JsonDataType type);
  /**
   * Returns the text representation of a node type.
   */
  static const char* typeToString(JsonNodeType type);
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
  virtual NodeJson& operator [](size_t index);
public:
  inline void add(NodeJson *value) {
    _array.push_back(value);
  }
  virtual void addAsString(std::string &jsonString, int indent, int levelbool,
      bool needsPrefix = true) const;
  virtual size_t addNeededBytes(size_t &needed, int indent, int level,
      bool needsPrefix = true) const;
  virtual std::vector<NodeJson*>* array(bool throwException = true);
  virtual NodeJson* byIndex(int index, bool throwException = false);
  virtual const NodeJson* byIndexConst(int index,
      bool throwException = false) const;
  virtual JsonDataType dataType() const;
  inline void reserve(size_t addittionalSize) {
    _array.reserve(_array.size() + addittionalSize);
  }
  virtual std::string toString(int maxLength = 40) const;
};

/**
 * @brief Stores a map in the Json data tree.
 */
class MapJson: public NodeJson {
protected:
  std::map<std::string, NodeJson*> _map;
public:
  MapJson();
  virtual ~MapJson();
  virtual NodeJson& operator [](const char *attribute);
public:
  void add(const char *name, NodeJson *item, bool maskMetaCharacters = true);
  virtual void addAsString(std::string &jsonString, int indent, int level,
      bool needsPrefix = true) const;
  virtual size_t addNeededBytes(size_t &needed, int indent, int level,
      bool needsPrefix = true) const;
  virtual NodeJson* byAttribute(const char *attribute, bool throwException =
      false);
  virtual const NodeJson* byAttributeConst(const char *attribute,
      bool throwException = false) const;
  virtual std::string checkStructure(NameAndType mandatory[],
      NameAndType optional[] = nullptr, bool mustComplete = false) const;
  virtual JsonDataType dataType() const;
  virtual std::map<std::string, NodeJson*>* map(bool throwException = true);
  bool erase(const char *attribute, bool deleteNode = true);
  bool hasAttribute(const char *attribute) const;
  virtual std::string toString(int maxLength = 40) const;
};
/**
 * @brief Stores a value in the Json data tree.
 */
class ValueJson: public NodeJson {
protected:
  std::string _value;
  JsonDataType _dataType;
public:
  ValueJson(JsonDataType, const char *value);
  ~ValueJson();
public:
  virtual void addAsString(std::string &jsonString, int indent, int level,
      bool needsPrefix = true) const;
  virtual size_t addNeededBytes(size_t &needed, int indent, int level,
      bool needsPrefix = true) const;
  virtual bool asBool() const;
  virtual double asDouble(double defaultValue = UNDEF_DOUBLE) const;
  virtual int asInt(int defaultValue = UNDEF_INT) const;
  virtual const char* asString() const;
  virtual JsonDataType dataType() const;
  virtual bool isNull() const;
  virtual std::string toString(int maxLength = 40) const;
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
  Logger &_logger;
  TokenInfo _token;
  std::string _error;
public:
  JsonReader(Logger &logger);
  virtual ~JsonReader();
public:
  /**
   * Returns the last error found while parsing.
   * @return "": No error. Otherwise: the error message.
   */
  const std::string& lastError() const;
protected:
  TokenType next();
public:
  /**
   * Parses a stream (file or string) into a JsonNode tree.
   * @param stream The text data to parse.
   * @return <em>nullptr</em>: error occurred. Otherwise: the root of the tree. It must deleted by the caller.
   */
  NodeJson* parse(LinesStream &stream);
protected:
  ArrayJson* parseArray();
  MapJson* parseMap();
};

} /* namespace cppknife */

#endif /* TEXT_NODEJSON_HPP_ */
