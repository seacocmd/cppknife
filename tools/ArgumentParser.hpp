/*
 * ArgumentParser.hpp
 *
 *  Created on: 17.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TOOLS_ARGUMENT_PARSER_HPP_
#define TOOLS_ARGUMENT_PARSER_HPP_

namespace cppknife {
enum DataType {
  DT_UNDEF,
  DT_NAT,
  DT_INT,
  DT_DOUBLE,
  DT_STRING,
  DT_SIZE,
  DT_SIZE_INT,
  DT_PATTERN,
  DT_BOOL,
  DT_DATE,
  DT_DATETIME,
  DT_FILE,
  DT_DIRECTORY,
  DT_FILE_OR_DIRECTORY,
  DT_FILE_PATTERN,
  DT_MODE,
  DT_REGEXPR
};
typedef const char **StringArray;
/**
 * That exception signals an argument parser error.
 */
class ArgumentException {
private:
  std::string _message;
public:
  ArgumentException(std::string message);
  ArgumentException(const char *message, const char *arg1 = nullptr,
      const char *arg2 = nullptr);
  ArgumentException(const ArgumentException &other) :
      _message(other._message) {
  }
  ArgumentException&
  operator=(const ArgumentException &other) {
    _message = other._message;
    return *this;
  }
public:
  const std::string& message() const {
    return _message;
  }
};

/**
 * Specification of a program argument (option or other argument).
 */
class Parameter {
private:
  std::string _name;
  std::string _longname;
  std::string _shortname;
  const char *_defaultValue;
  std::string _help;
  DataType _dataType;
  std::string _examples;
  bool _multiple;
  std::string _format;
  std::string _formatDescription;
public:
  /**
   * Constructor.
   * @param longname The name of the argument.
   *   If preceeded by "--" this implies an option and the name is that without "--".
   * @param shortname The short name of the argument.
   *   If preceeded by "-" this implies an option and the name is that without "-"
   *   only if name is nullptr.
   * @param dataType The data type of the argument.
   * @param help The text used in the usage message.
   * @param defaultValue If the parameter has no concrete program argument that value.
   *  is returned in asString(), asBool()...
   * @param examples <em>nullptr</em> or a list of examples separated by '|', e.g. "0|-33|99"
   * @param multiple true: The argument can occure multiple times.
   */
  Parameter(const char *longname, const char *shortname, DataType dataType,
      const char *help, const char *defaultValue = nullptr,
      const char *examples = nullptr, bool multiple = false);
  virtual
  ~Parameter();
  Parameter(const Parameter &other);
  Parameter&
  operator=(const Parameter &other);
public:
  DataType dataType() const {
    return _dataType;
  }
  /**
   * Returns the default value.
   */
  const char*
  defaultValue() const {
    return _defaultValue;
  }
  const std::string format() const {
    return _format;
  }
  const std::string formatDescription() const {
    return _formatDescription;
  }
  /**
   * Tests whether the parameter is an option: Than the longname or the shortname starts with '-'.
   */
  bool isOption() const {
    bool rc = (!_longname.empty() && _longname[0] == '-')
        || (!_shortname.empty() && _shortname[0] != '-');
    return rc;
  }
  const char*
  longname() const {
    return _longname.c_str();
  }
  /**
   * Returns whether the parameter can be used multiple times.
   */
  bool multiple() const {
    return _multiple;
  }
  /**
   * Returns the name.
   */
  std::string name() const {
    return _name;
  }
  void setFormat(const char *format, const char *formatDescription) {
    _format = format;
    _formatDescription = formatDescription;
  }
  const char*
  shortname() const {
    return _shortname.c_str();
  }
  /**
   * Builds the message describing the parameter used for usage.
   * @param indent The string used for one indent step.
   */
  std::string
  usage(const char *indent) const;
public:
  static const char*
  buildExample(DataType dataType, const char *examples);
};
struct ArgVector {
  ArgVector(int argc, char **argv) :
      _argc(argc), _argv(const_cast<const char**>(argv)) {
  }
  int _argc;
  const char **_argv;
};
/**
 * The concrete program argument (option or other argument).
 */
class Argument {
protected:
  std::string _name;
  const Parameter *_parameter;
  std::vector<std::string> _values;
public:
  Argument();
  Argument(const Parameter *parameter, std::string &value);
public:
  void addValue(const char *value) {
    _values.push_back(value);
  }
  bool
  asBool();
  double
  asFloat(size_t index = 0) const;
  int
  asInt(size_t index = 0) const;
  int
  asNat(size_t index = 0) const;
  std::regex asRegExpr(size_t index = 0) const;
  int64_t
  asSize(size_t index = 0) const;
  const std::string&
  asString(size_t index = 0) const;
  inline size_t countValues() const {
    return _values.size();
  }
  const Parameter*
  parameter() const {
    return _parameter;
  }
  bool
  validate();
  const std::string&
  value(size_t index = 0) const {
    return _values[index];
  }
protected:
  void
  checkIndex(size_t index) const;
};
void
argumentReleaser(void *object);
void
parameterReleaser(void *object);
void
argumentParserReleaser(void *object);
/**
 * Manages the program arguments.
 */
class ArgumentParser {
protected:
  std::string _name;
  std::string _description;
  std::vector<Parameter*> _parameters;
  std::map<std::string, ArgumentParser*> _subParsers;
  std::map<std::string, Argument*> _arguments;
  std::string _usageMessage;
  Storage _argumentStorage;
  Storage _parameterStorage;
  Storage _argumentParserStorage;
  Logger *_logger;
  ArgumentParser *_activeSubParser;
  ArgumentParser *_rootParser;
public:
  ArgumentParser(const char *name, Logger *logger,
      const char *description = "");
  virtual
  ~ArgumentParser();
private:
  ArgumentParser(const ArgumentParser &other);
  ArgumentParser&
  operator=(const ArgumentParser &other);
public:
  /**
   * Adds a parameter specification.
   * @param longname The name of the argument.
   *   If preceeded by "--" this implies an option and the name is that without "--".
   * @param shortname The short name of the argument.
   *   If preceeded by "-" this implies an option and the name is that without "-"
   *   only if name is nullptr.
   * @param dataType The data type of the argument.
   * @param help The text used in the usage message.
   * @param defaultValue If the parameter has no concrete program argument that value
   *  is returned in asString(), asBool()...
   * @param examples <em>nullptr</em> or a list of examples separated by '|', e.g. "0|-33|99"
   * @param multiple <em>true</em>: The argument can be used multiple times.
   */
  void
  add(const char *longname, const char *shortname, DataType dataType,
      const char *help, const char *defaultValue = nullptr,
      const char *examples = nullptr, bool multiple = false);
  /**
   * Adds a sub command.
   * @param name The name of the sub command.
   * @param help Description of the the sub command.
   * @param values A comma separated list of possible values, e.g. "print,info,draw"
   */
  void
  addMode(const char *name, const char *help, const char *values);
  /**
   * Adds a parser of a sub command.
   * @param mode The name of the parameter with the sub command.
   * @param value The sub parser is used if the parameter has that value.
   * @param subParser The sub parser to store.
   */
  void
  addSubParser(const char *mode, const char *value, ArgumentParser &subParser);
  /**
   * Returns the current value of the program argument as bool.
   * @param name: the name of the argument.
   * @param defaultValue: The result if the parameter is not in the program arguments.
   * @return The value of the argument.
   */
  int
  asBool(const char *name, int defaultValue = -1) const;
  /**
   * Returns the current value of the program argument as double.
   * @param name The name of the argument.
   * @param defaultValue The result if the parameter is not in the program arguments.
   * @param index: 0 or the index (when multiple values are allowed).
   * @return The value of the argument.
   */
  double
  asDouble(const char *name, double defaultValue = 1E100,
      size_t index = 0) const;
  /**
   * Returns the current value of the program argument as an integer.
   * @param name: the name of the argument.
   * @param defaultValue: The result if the parameter is not in the program arguments.
   * @param index
   * @return The value of the argument.
   */
  int
  asInt(const char *name, int defaultValue = -0x7fffffff,
      size_t index = 0) const;
  /**
   * Returns the current value of the program argument as regular expression.
   * @param name: the name of the argument.
   * @param defaultValue: The result if the parameter is not in the program arguments.
   * @param index: 0 or the index (when multiple values are allowed).
   * @return The value of the argument.
   */
  std::regex
  asRegExpr(const char *name, const char *defaultValue = nullptr, size_t index =
      0) const;
  /**
   * Returns the current value of the program argument as (file) size.
   * @param name: the name of the argument.
   * @param defaultValue: The result if the parameter is not in the program arguments.
   * @param index: 0 or the index (when multiple values are allowed).
   * @return The value of the argument.
   */
  int64_t
  asSize(const char *name, int64_t defaultValue, size_t index = 0) const;
  /**
   * Returns the current value of the program argument.
   * @param name: the name of the argument.
   * @param defaultValue <em>nullptr</em> or the value used if the parameter is not used.
   * @param index: 0 or the index (when multiple values are allowed).
   */
  const char*
  asString(const char *name, const char *defaultValue = nullptr, size_t index =
      0) const;
  /**
   * Returns the count of values of a given argument (&gt; 1 on parameters with multiple values).
   * @param name The name of the argument to inspect.
   * @return The count of values of the argument <em>name</em>.
   */
  size_t countValuesOf(const char *name) const {
    auto argument = findArgument(name);
    auto rc = argument == nullptr ? 0 : argument->countValues();
    return rc;
  }
  /**
   * Tests whether the mode with a given name has a given value.
   * @param name The name of sub command.
   * @param value The expected value of the sub command.
   * @return true: the mode has the given value.
   */
  bool
  isMode(const char *name, const char *value);
  bool
  checkArguments();
  std::string modeDescription(const char *indent) const;
  /**
   * Parses the program arguments, checks that and stores that as parameters.
   * @param argVector Contains the modifiable program argument info.
   * @return false: an error has been occurred.
   */
  bool
  parseAndCheck(ArgVector &argVector);
  /**
   * Sets the regular expression describing the syntax of an string argument.
   * @param name The name of the argument.
   * @param format The format.
   * @param formatDescription The error message if the syntax is wrong. Placeholder: %v: current value.
   */
  void setParameterFormat(const char *name, const char *format,
      const char *formatDescription);
  /**
   * Builds a string with the usage notice.
   * @param message <em>nullptr</em> or the error message.
   * @param subParser <em>nullptr</em> or: the usage message of that will be appended
   * @param allSubParsers <em>true</em>: the usage message of all sub parsers will be appended
   */
  std::string
  usage(const char *message, ArgumentParser *subParser = nullptr,
      bool allSubParsers = false) const;
  /**
   * Returns the usage message.
   * @return The usage message.
   */
  const std::string& usageMessage() const {
    return _usageMessage;
  }
protected:
  void
  addArgument(const Parameter *parameter, std::string value);
  void
  addDefaultArguments();
  const Parameter*
  buildOptionArgument(ArgVector &argVector, std::string &value);
  void
  findOptions(std::vector<Parameter*> &nonOptions,
      std::map<std::string, Parameter*> &options);
  Argument*
  findArgument(const char *name) const;
  const Parameter*
  findByLongname(const char *name) const;
  Parameter*
  findByName(const char *name) const;
  const Parameter*
  findByShortname(const char *name) const;
  bool
  parse(ArgVector &argVector);
};
void
parameterRelease(void *object);
void
argumentRelease(void *object);
void
argumentParserRelease(void *object);
} /* namespace polygeo */

#endif /* TOOLS_ARGUMENT_PARSER_HPP_ */
