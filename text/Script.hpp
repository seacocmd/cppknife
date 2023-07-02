/*
 * Script.hpp
 *
 *  Created on: 09.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_SCRIPT_HPP_
#define TEXT_SCRIPT_HPP_

namespace cppknife {

class SearchEngine;
class LineBuffer;

/// Stores a parameter element in the <em>Search Engine Script Language</em>: it is expressed as <em>&lt;name>=&lt;value></em>.
/**
 * Stores a parameter element in <em>Search Engine Script Language</em>: it is expressed as <em>&lt;name>=&lt;value></em>.
 */
class ParameterInfo {
public:
  std::string _name;
  std::string _value;
  enum DataType {
    DT_INT, DT_STRING
  } _dataType;
  ParameterInfo(const char *name, DataType type) :
      _name(name), _dataType(type) {
  }
};

/// Stores an amount of allowed parameter elements.
/**
 * Stores an amount of allowed parameter elements.
 */
class ParameterSet {
protected:
  std::map<std::string, ParameterInfo*> _parameters;
public:
  ParameterSet();
  ParameterSet(ParameterInfo *parameter);
  ParameterSet(ParameterInfo *parameters[]);
  ~ParameterSet();
public:
  void add(ParameterInfo &parameter);
  int asInt(const char *name, int defaultValue = -1) const;
  ParameterInfo* find(const char *name) const;
  std::string populate(const char *definition, std::string &key);
};
/// Stores the two block end line numbers from the if statement.
/**
 * Stores the two block end line numbers from the if statement.
 */
struct IfData {
  // The index below the else statement in _lines[]
  int _elseIndex;
  // The index below the endif statement in _lines[]
  int _endifIndex;
public:
  IfData() :
      _elseIndex(0), _endifIndex(0) {
  }
  IfData(int elseIndex, int endIndex) :
      _elseIndex(elseIndex), _endifIndex(endIndex) {
  }
};
class SearchResult;
class SearchExpression;
/// Extends the <em>Parser</em> for processing the <em>Search Engine Script Language</em>.
/**
 * Extends the <em>Parser</em> for processing the <em>Search Engine Script Language</em>.
 */
class SearchParser: public Parser {
public:
  enum KeyWord {
    KW_ASSERT, // 0
    KW_CALL,
    KW_COPY,
    KW_DELETE,
    KW_ELSE,
    KW_ENDIF,
    KW_ENDSCRIPT,
    KW_ENDWHILE,
    KW_EXIT,
    KW_IF,
    KW_INSERT, // 10
    KW_LEAVE,
    KW_LOAD,
    KW_LOG,
    KW_MARK,
    KW_MOVE,
    KW_RANGE,
    KW_REPLACE,
    KW_SCRIPT,
    KW_SELECT,
    KW_STOP,
    KW_STORE,
    KW_WHILE,
  };
  enum Operators {
    // !=, -eq, -ge, -gt, -le, -lt, -ne, :=, <, <=, =, ==, >, >=, ~=
    OP_UNKNOWN,
    OP_NE,
    OP_EQS,
    OP_GES,
    OP_GTS,
    OP_LES,
    OP_LTS,
    OP_NES,
    OP_ASSIGN_NUMERIC,
    OP_LT,
    OP_LE,
    OP_ASSIGN,
    OP_EQ,
    OP_GT,
    OP_GE,
    OP_SIMILAR
  };
public:
  static const int TT_BUFFER_EXPRESSION = TT_SPECIAL_1; // 9
  static const int TT_BUFFER = TT_BUFFER_EXPRESSION + 1; // 10
  static const int TT_VARIABLE = TT_BUFFER + 1; // 11
  static const int TT_STRING2 = TT_VARIABLE + 1; // 12
  static const int TT_PATTERN = TT_STRING2 + 1; // 13
  static const int TT_PARAMETER_DEFINITION = TT_PATTERN + 1; // 16
  static const int TT_FUNCTION = TT_PARAMETER_DEFINITION + 1; // 17

public:
  static const std::vector<std::string> _operators;
  static const std::regex _regexString;
  static const std::regex _regexPattern;
  static const std::regex _regexBufferExpression;
  static const std::regex _regexBuffer;
  static const std::regex _regexVariable;
  static const std::regex _regexVariable2;
  static const std::regex _regexParameterDefinition;
  static const std::regex _regexFunction;
  static const std::regex _regexOperator1;
  static const std::regex _regexOperator2;
  /// for single commands:
  static const std::regex _regexBackReference;
  static const std::regex _regexHereDocument;
  static const std::regex _regexAbsolutePosition;
  static const std::regex _regexFunctions;
  static const std::regex _regexFrom;
  static const std::regex _regexAssignment;
  static const std::vector<std::string> _keywords;
public:
  SearchParser(Logger &logger);
  virtual ~SearchParser();
public:
  LineBuffer* asBufferName(SearchEngine &engine, bool mustExist = true);
  /**
   * Splits a string into a position.
   * @param value The value to split.
   * @param position OUT: the decoded position.
   */
  void asPosition(const std::string &value, BufferPosition &position) const;
  /**
   * Return the current token as <em>SearchExpression</em>.
   * @param expression OUT: the search expression.
   */
  void asPattern(SearchExpression &expression);
  static std::string nameOfBuffer(const std::string &name);
  /**
   * Parses the buffer name and return the assigned buffer.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   * @param engine The assigned search engine.
   * @param mustBePresent <em>true</em>: if there is not a buffer name an error occurs.
   * @param bufferMustExist <em>true</em>: if the buffer does not exist an error occurs.
   * @return <em>nullptr</em>: The buffer does not exists. Otherwise: the buffer.
   */
  LineBuffer* parseBuffer(bool testOnly, SearchEngine *engine,
      bool mustBePresent, bool bufferMustExist);
  /**
   * Parses an integer if that exists.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   * @param name The name of the expected string (for error messages).
   * @param defaultValue If <em>nullptr</em>: if there is no string to parse an error occurs.
   * @param mustExist <em>true</em>: if the buffer does not exist an error occurs.
   * @return <em>nullptr</em>: the default value if no string is there. Otherwise the parsed string.
   */
  int parseInt(bool testOnly, const char *name, bool mustExist,
      int defaultValue = 0);
  /**
   * Parses a search expression if that exists.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   * @param name The name of the expected patterns (for error messages).
   * @param mustExist If <em>true</em> and there is no pattern an error occurs.
   * @param searchExpression OUT: the found search expression.
   * @return <em>true</em>: A pattern has been found.
   */
  bool parsePattern(bool testOnly, const char *name, bool mustExist,
      SearchExpression &searchExpression);
  /**
   * Parses a string if that exists.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   * @param name The name of the expected string (for error messages).
   * @param mustExist If <em>true</em>: if there is no position to parse an error occurs.
   * @param position OUT: The result of the parsed position.
   * @return <em>true</em>: a position has been found.
   */
  bool parsePosition(bool testOnly, const char *name, bool mustExist,
      BufferPosition &position);
  /**
   * Parses a string if that exists.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   * @param name The name of the expected string (for error messages).
   * @param defaultValue If <em>nullptr</em>: if there is no string to parse an error occurs.
   * @param needed If <em>false</em> and <em>defaultValue == nullptr</em>: no error occurs.
   * @return <em>nullptr</em>: the default value if no string is there. Otherwise the parsed string.
   */
  std::string parseString(bool testOnly, const char *name,
      const char *defaultValue = nullptr, bool needed = true);
};

/// Stores the state of the nesting block statements.
/**
 * Stores the state of the nesting block statements.
 */
class BlockStackEntry {
public:
  enum BlockType {
    BT_UNDEF, BT_IF_THEN, BT_IF_ELSE, BT_WHILE
  };
public:
  BlockType _type;
  size_t _lineIndex;
public:
  BlockStackEntry();
  BlockStackEntry(BlockType type, size_t _lineIndex);
};
class FunctionEngine;

/// Represents a single script.
/**
 * Represents a single script.
 *
 * A script is an list of statements.
 */
class Script: public LineList {
  friend SearchEngine;
protected:
  std::string _name;
  SearchEngine &_engine;
  SearchParser _parser;
  /// When we start to execute a statement this index points behind the current.
  size_t _indexNextStatement;
  /// key: variable name, e.g. "$(pos)" value: variable value
  std::map<std::string, std::string> _variables;
  /// key: name (without $) value: buffer
  std::map<std::string, LineBuffer*> _buffers;
  /// Stores the line numbers of the two block ends. Key: line number of if
  std::map<int, IfData*> _ifData;
  // Key: line number of "else" or "while" Value: line number of line below the "endif" or "endwhile"
  std::map<int, size_t> _singleEndData;
  std::vector<BlockStackEntry*> _openBlocks;
  LineBuffer *_currentBuffer;
  std::vector<LineBuffer*> _bufferStack;
  FunctionEngine *_functionEngine;
  /// <em>true</em>: the current statement has a numerical context: empty variables are replaced by '0'.
  bool _numericalContext;
  bool _alreadyTested;
public:
  Script(const char *name, SearchEngine &parent, Logger &logger);
  virtual ~Script();
public:
  void _check();
  /**
   * Returns the current token interpreted as buffer name.
   */
  std::string asBuffer() const;
  /**
   * Handles the "assert" statement.
   */
  void assertStatement(bool testOnly);
  /**
   * Returns the current token interpreted as string.
   */
  std::string asString() const;
  /**
   * Handles the assignment statements.
   */
  void assignment(bool testOnly);
  /**
   * Handles the assignment statements for numeric data (defined by ":=").
   */
  void assignmentNumeric(const std::string &name, bool testOnly);
  /**
   * Tests the script.
   */
  void check();
  /**
   * Handles the "call" statement.
   */
  void call(bool testOnly);
  /**
   * Handles the "copy" statement.
   */
  void copy(bool testOnly);
  /**
   * Creates a buffer.
   * @param name The buffer's name. May be start with '$' or not.
   * @return The buffer.
   */
  LineBuffer* createBuffer(const char *name);
  /**
   * Handles the "delete" statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void deleteStatement(bool testOnly);
  /**
   * Handles the "else" statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void elseStatement(bool testOnly);
  /**
   * Handles the "endif" statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void endIf(bool testOnly);
  /**
   * Handles the "exit" statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void exitStatement(bool testOnly);
  /**
   * Handles the "endscript" statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void endScript(bool testOnly);
  /**
   * Handles the "endwhile" statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void endWhile(bool testOnly);
  /**
   * Logs an error and stops the script if specified.
   * @param message The error message.
   */
  void error(const char *message);

  /**
   * Finds the end of a statement block.
   * @param line The index of the first line of the block.
   * @param idStatement The id of the statement starting the block, e.g. KW_IF
   * @param idEnd1 The id of the first block end, e.g. KW_ELSE
   * @param idEnd2 The id of the second block end, e.g. KW_ENDIF
   * @param idFound: OUT: the id of the block end: idEnd1 or idEnd2
   * @return the line index of the block end
   */
  size_t findEndOfBlock(int line, int idStatement, int idEnd1, int idEnd2,
      int &idFound);

  /**
   * Returns the buffer with a given name (if available).
   * @param name The buffer name. If null the current buffer is taken.
   * @return <em>nullptr</em>: buffer is unknown. Otherwise: the buffer.
   */
  LineBuffer* getBuffer(const char *name = nullptr);
  /**
   * Returns the value of a condition (used in if and while statement).
   * @param testOnly <em>true</em>: the syntax is tested only.
   */
  bool getCondition(bool testOnly);
  /**
   * Returns the value of a boolean condition (used in if and while statement):
   * A number, a string or a comparison of numbers or strings.
   * @param testOnly <em>true</em>: the syntax is tested only.
   * @param tokenYetFetched <em>true</em>: the token is already parsed.
   */
  bool getConditionAsBool(bool testOnly, bool tokenYetFetched);
  /**
   * Returns the value of a "similar" condition (matches a regular expression or not):
   * A number, a string or a comparison of numbers or strings.
   * @param testOnly <em>true</em>: the syntax is tested only.
   * @param op1: the value of the operand to test.
   */
  bool getConditionAsSimilar(bool testOnly, const std::string &op1);
  /**
   * Handles a text: That is a string, a buffer content, a buffer expression or a here document.
   * @param testOnly <em>true</em>: the syntax is tested only.
   * @param contents OUT: the lines of the text as vector.
   */
  void getText(bool testOnly, std::vector<std::string> &contents);
  /**
   * Tests whether a parameter definition is at the current parser position.
   * @param parameterSet IN/OUT: IN: The allowed parameters Out: the value of the parameter.
   * @return Empty string if there is no parameter definition. Otherwise the name of the parameter.
   */
  std::string hasParameter(ParameterSet &parameterSet);
  /**
   * Handles the if else endif statements.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void ifStatement(bool testOnly);
  /**
   * Returns the index of the statement which will executed after the current.
   */
  inline int indexNextStatement() {
    return _indexNextStatement;
  }
  /**
   * Replaces the variable notation by the variable contents.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   * @param line The line to process. If <em>nullptr</em> than <em>_input._unprocessed</em> is used.
   * @param numericContext <em>true</em>: if the variable does not exist or the variable is empty:
   *  <em>0</em> is take as replacement. That is necessary for a correct syntax.
   */
  void interpolate(bool testOnly, std::string *line = nullptr,
      bool numericContext = false);
  /**
   * Handles the "insert" statements.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void insert(bool testOnly);
  /**
   * Handles the log statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void log(bool testOnly);
  /**
   * Handles the leave statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void leave(bool testOnly);
  /**
   * Handles the load statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void load(bool testOnly);
  /**
   * Handles the mark statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void mark(bool testOnly);
  /**
   * Handles the move statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void move(bool testOnly);
  /**
   * Tests or executes one statement.
   * @param testOnly <em>true</em>: the statement should be tested not executed.
   */
  void oneStatement(bool testOnly);
  /**
   * Returns the precedence of an operator.
   * @param theOperator The operator to inspect.
   * @return 1 for '+' and '-', 2 for '*' and '/', '%' and ':'
   */
  int precedence(char theOperator);
  /**
   * Handles the replace statement.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void replace(bool testOnly);
  /**
   * Runs the script.
   */
  void run();
  /**
   * Handles the "script" statement.
   */
  void script(bool testOnly);
  /**
   * Searches a <em>searchExpr</em> in in the <em>buffer</em>.
   * @param buffer The buffer to inspect.
   * @param searchExpr Defines what to search.
   * @param searchResult OUT: The search position.
   * @param setPosition <em>true</em>The current position will be changed
   * @return <em>true</em>Success.
   */
  bool searchBuffer(LineBuffer &buffer, SearchExpression &searchExpr,
      SearchResult &searchResult, bool setPosition = true);
  /**
   * Handles the "stop" statement.
   */
  void select(bool testOnly);
  /**
   * Defines the current buffer.
   * In some commands the buffer is optional. In that case the current buffer is used.
   * @param name The buffer's name, e.g. ~csv
   * @param value The new value.
   */
  void setVariable(const std::string &name, const std::string &value);
  /**
   * Handles the "stop" statement.
   */
  void stop(bool testOnly);
  /**
   * Handles the "store" statement.
   */
  void store(bool testOnly);
  /**
   * Determines the value of a buffer expression: some lines from a buffer.
   */
  std::string valueOfBufferExpression();
  /**
   * Returns the value of a variable as double value.
   * @param name The name of the variable, e.g. "$(line)".
   * @param quiet <em>true</em>: an error is not logged if the variable does not exist.
   * @return 0: unknown variable. Otherwise: the value of the variable.
   */
  double variableAsDouble(const std::string &name, bool quiet = false);
  /**
   * Returns the value of a variable.
   * @param name The name of the variable, e.g. "$(line)".
   * @param quiet <em>true</em>: an error is not logged if the variable does not exist.
   * @return "": unknown variable. Otherwise: the value of the variable.
   */
  std::string variableAsString(const std::string &name, bool quiet = false);
  /**
   * Returns whether a (local or global) variable exists.
   * @param name: the name of the variable: <em>id</em> or <em>$(id)</em>
   * @return <em>true</em>: the variable exists.
   */
  bool variableExists(const char *name);
  /**
   * Handles the while endwhile statements.
   * @param testOnly testOnly <em>true</em>: the statement should be tested not executed.
   */
  void whileStatement(bool testOnly);
};

} /* namespace cppknife */

#endif /* TEXT_SCRIPT_HPP_ */
