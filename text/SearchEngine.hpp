/*
 * SearchEngine.hpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_SEARCHENGINE_HPP_
#define TEXT_SEARCHENGINE_HPP_

namespace cppknife {

class SearchEngine;

class LineRange {
protected:
  /// Index of the first line.
  size_t _start;
  /// Index of the last line (including).
  size_t _end;
  SearchEngine &_parent;
public:
  LineRange(SearchEngine &parent);
public:
  bool find(std::regex start, std::regex end);
};

class LineBuffer: public LineList {
protected:
  SearchEngine &_engine;
public:
  LineBuffer(Logger &logger, SearchEngine &engine);
  virtual ~LineBuffer();
};
class SearchResult;
/**
 * Combines the search with simple strings and with regular expressions.
 */
class SearchEngine: public LineBuffer {
  friend LineRange;
  friend Script;
protected:
  // Key: name (without ~) value: buffer
  std::map<std::string, LineBuffer*> _buffers;
  std::map<std::string, Script*> _scripts;
  /// The stack of the scripts: each <em>call</em> statement enlarge that.
  std::vector<std::string> _scriptStack;
  /// The value of <em>Script::_indexNextStatement</em> at the moment of the call.
  std::vector<int> _lineNoStack;
  Script *_currentScript;
  FILE *_trace;
  std::string _traceName;
  std::string _lastHit;
  std::map<std::string, std::string> _globalVariables;
public:
  SearchEngine(Logger &logger);
  virtual ~SearchEngine();
public:
  /**
   * Adds a local script to the map.
   * @param script The script to add.
   */
  void addScript(Script *script);
  /**
   * Creates a buffer for the current script.
   * @param name The name of the buffer to create.
   * @returns The created buffer.
   */
  LineBuffer* createBuffer(const char *name);
  Script* currentScript(const char *object, const char *name);
  /**
   * Defines a variable in the current script.
   * @param name The name of the variable: myVariable or $(myVariable).
   * @param value The value of the variable.
   */
  void defineVariable(const char *name, const char *value);
  /**
   * Returns a named buffer.
   * @param name The buffer name. If <em>nullptr</em> than <em>~_main</em> is taken.
   * @return <em>nullptr</em>: buffer is unknown. Otherwise: the buffer.
   */
  LineBuffer* getBuffer(const char *name = nullptr);
  /**
   * Returns the value of a global variable or <em>nullptr</em>.
   * @param name The variable name.
   * @return <em>nullptr</em>: variable not found. Otherwise: the value of the variable.
   */
  const char* globalVariable(const char *name) const;
  /**
   * Loads a script from a file into the instance.
   * @param scriptName The name of the script.
   * @param filename The name of the file with the script.
   */
  void loadScript(const char *scriptName, const char *filename);
  /**
   * Removes the last pushed script from the list.
   * @param script the script to add.
   */
  void popScript();
  /**
   * Put the script on top of the script stack and make it to the current script.
   * @param script the script to add.
   * @param lineNo the line number of the call.
   */
  void pushScript(Script *script, int lineNo);
  /**
   * Select a loaded script as the "current script".
   * @param scriptName The name of the script.
   */
  void selectScript(const char *scriptName);
  /**
   * Sets a global variable.
   * @param name The name of the variable.
   * @param value The value of the variable.
   */
  void setGlobalVariable(const std::string &name, const std::string &value);
  /**
   * (Re-)Opens the trace file.
   * @param filename The name of the trace file: '-': <em>stdout</em>. If <em>nullptr</em>: close tracefile.
   * @param append <em>true</em>The trace will be appended to the file.
   */
  void setTrace(const char *filename, bool append = false);
  /**
   * Returns a script given by name.
   * @param name The script name
   * @return <em>nullptr</em>: not found Otherwise: the script.
   */
  Script* scriptByName(const std::string &name);
  /**
   * Tests the syntax of the script and run it.
   * @param scriptName The name of the script. If <em>nullptr</em> the current script is used.
   * @return Exit code: 0 on success.
   */
  int testAndRun(const char *scriptName = nullptr);
  /**
   * Returns whether a (global) variable exists.
   * @param name: the name of the variable: <em>id</em> or <em>$(id)</em>
   * @return <em>true</em>: the variable exists.
   */
  bool variableExists(const char *name);
};

} /* namespace cppknife */

#endif /* TEXT_SEARCHENGINE_HPP_ */
