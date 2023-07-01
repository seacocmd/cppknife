/*
 * Matcher.hpp
 *
 *  Created on: 11.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_MATCHER_HPP_
#define CORE_MATCHER_HPP_

namespace cppknife {

class Hit {
public:
  Hit();

  inline int getEnd() const {
    return _end;
  }

  inline void setEnd(int end) {
    _end = end;
  }

  inline int getStart() const {
    return _start;
  }

  inline void setStart(int start) {
    _start = start;
  }
private:
  int _start;
  int _end;
};

class Matcher {
public:
  Matcher(bool notPattern);
  virtual ~Matcher();
public:
  /**
   * Stores the pattern in a preprocessed form for efficient searching.
   * @param pattern: the pattern to compile.
   * @return <em>false</em>: an error has occurred.
   */
  virtual bool compile(const char *pattern) = 0;
  /**
   * Tests whether a string matches the pattern stored in the instance.
   *
   * This method is anchored: search always starts at the begin of <em>toTest</em>.
   * @param toTest  The string to test.
   * @param toTestLength -1: <em>strlen(toTest)</em> is used.<br>
   *   Otherwise: the length of <em>toTest</em>.
   * @param hit   OUT: hit data. May be NULL
   * @return <em>true</em>: ! _notPattern: the string matches<br>
   *   _notPattern: the string matches not<br>
   *   <em>false</em>: otherwise
   */
  virtual bool match(const char *toTest, int toTestLength = -1,
      Hit *hit = NULL) const = 0;
  /** @brief Tests whether a string matches the patterns.
   * @param toTest  The string to test.
   * @return    <em>true</em>: the string matches
   */
  virtual bool match(const std::string &toTest) {
    return match(toTest.c_str(), toTest.size());
  }
  /**
   * Tests whether a string matches the pattern stored in the instance.
   *
   * This method is not anchored: the string may be anywhere in the text.
   * @param toTest  The string to test.
   * @param toTestLength -1: <em>strlen(toTest)</em> is used.<br>
   *   Otherwise: the length of <em>toTest</em>.
   * @param hit   OUT: hit data. May be NULL
   * @param greedy <em>true:</em>: the all quantor ("*") eats as much text
   *   as possible. <br>
   *   <em>false</em>: the all quantor ("*") eats as little text
   *   as possible.
   * @return <em>true</em>: ! _notPattern: the string matches<br>
   *   _notPattern: the string matches not<br>
   *   <em>false</em>: otherwise
   */
  virtual bool search(const char *toTest, int toTestLength = -1,
      Hit *hit = NULL, bool greedy = false) const = 0;
  inline bool search(const std::string &toTest, Hit *hit = NULL, bool greedy =
      false) {
    return search(toTest.c_str(), toTest.size(), hit, greedy);
  }
public:
  inline bool ignoreCase() const {
    return _ignoreCase;
  }

  inline void setIgnoreCase(bool ignoreCase) {
    _ignoreCase = ignoreCase;
  }

  inline bool isNotPattern() const {
    return _notPattern;
  }

  inline void setNotPattern(bool notPattern) {
    _notPattern = notPattern;
  }

protected:
  bool _findAll;
  bool _ignoreCase;
  bool _notPattern;
};

class SimpleMatcher: public Matcher {
private:
  std::string _pattern;
  std::vector<std::string> _tokens;
public:
  SimpleMatcher();
  /**
   * Constructor.
   * @param pattern The search pattern.
   * @param notPattern The excluding pattern.
   */
  SimpleMatcher(const char *pattern, bool notPattern = false);
  virtual ~SimpleMatcher();
public:
  /**
   * Prepares the internals for efficient searching.
   * @param pattern: The pattern to search. Can contain wildcards '*' (any text).
   * @return			<em>true</em>: success<br>
   * 					<em>false</em>: error occurred
   */
  virtual bool compile(const char *pattern);
  void dump(FILE *fp, const char *prefix) const;
  virtual bool match(const char *toTest, int toTestLength = -1,
      Hit *hit = NULL) const;
  virtual bool search(const char *toTest, int toTestLength = -1,
      Hit *hit = NULL, bool greedy = false) const;
public:
  /**
   * Returns whether a pattern fits the given text.
   * @param pattern The string to search with wildcards '*'.
   * @param text The string to inspect.
   * @param patternLength The length of <em>pattern</em> or <em>-1</em> (c length).
   * @param textLength The length of <em>text</em> or <em>-1</em> (c length).
   * @return <em>true</em>: <b>pattern</b> hits <b>text</b>.
   */
  static bool matches(const char *pattern, const char *text, int patternLength =
      -1, int textLength = -1);
  /**
   * Returns the first index of a vector matching a given pattern.
   * @param pattern The pattern to test.
   * @param list The vector to inspect
   * @param startIndex: the first index of <em>list</em> to inspect.
   * @return: -1: nothing found Otherwise: the lowest index &gt;= startIndex where the pattern matches.
   */
  static int firstMatch(const char *pattern,
      const std::vector<std::string> &list, int startIndex = 0);
  /**
   * Returns only that elements of a vector matching a given pattern.
   * @param pattern The pattern to test.
   * @param list The vector to inspect.
   * @return A vector of that entries matching the <b>pattern</b>. May be empty.
   */
  static std::vector<std::string> filterMatches(const char *pattern,
      const std::vector<std::string> &list);
protected:
  /**
   * Search all tokens from a given index.
   *
   * This method may be recursive (if greedy).
   *
   * @param toTest The string to test.
   * @param toTestLength -1: <em>strlen(toTest)</em> is used.<br>
   *   Otherwise: the length of <em>toTest</em>.
   * @param from The index of the first token to find.
   * @param to The last index for searching.
   * @param hit OUT: hit data. May be NULL.
   * @param greedy <em>true</em>: the longest possible match will be found<br>
   *   <em>false</em>: the shortest possible match will be found
   * @return <em>true</em>: ! _notPattern: the name matches<br>
   *   _notPattern: the name matches not<br>
   *   <em>false</em>: otherwise
   */
  bool searchTokens(const char *toTest, int toTestLength, int from, int to,
      Hit *hit, bool greedy) const;
};

class PatternList {
private:
  std::string _patternString;
  // store of all patterns: the not patterns are at the bottom
  std::vector<SimpleMatcher*> _patterns;
public:
  PatternList();
  virtual ~PatternList();
public:
  /**
   * Frees the resources.
   */
  void clear();
  /** @brief Returns the number of entries in the list.
   * @return the count of patterns in the list
   */
  inline int count() const {
    return _patterns.size();
  }
  void dump(FILE *fp, const char *prefix) const;
  /** Tests whether the search is case insensitive
   * @return		<em>true</em>: the search is case insensitive
   */
  bool ignoreCase() const {
    bool rc = _patterns.size() == 0 ? false : _patterns[0]->ignoreCase();
    return rc;
  }
  /** Returns the original pattern string.
   * @return the string describing the patterns.
   */
  const char* patternString() const {
    return _patternString.c_str();
  }
  /**
   * Tests whether a string matches at least one of the patterns.
   *
   * definition: <br>
   * negative pattern: a pattern with the not condition<br>
   * positive pattern: an other pattern
   * <pre>Examples:
   * list: ";*.cpp;-test*;*.hpp"
   * returns true: "a.cpp" "xy.hpp"
   * returns false: "x.img", "test.cpp"
   * </pre>
   *
   * @param toTest The string to test.
   * @param toTestLength -1: <em>strlen(toTest)</em> will be used.<br>
   *   Otherwise: the length of <em>toTest</em>.
   * @return <em>true</em>: at least one pattern matches and no "not-pattern" matches<br>
   *  <em>false</em>: no pattern matches or at least one *  "not-pattern" matches
   */
  bool match(const char *toTest, int toTestLength = -1);
  inline bool match(const std::string &toTest) {
    return match(toTest.c_str(), toTest.size());
  }
  /**
   * Tests whether a string matches the pattern stored in the instance.
   *
   * This method is not anchored: the string may be anywhere in the text.
   * @param toTest  The string to test.
   * @param toTestLength -1: <em>strlen(toTest)</em> is used.<br>
   *   Otherwise: the length of <em>toTest</em>.
   * @param hit   OUT: hit data. May be NULL
   * @param greedy <em>true:</em>: the all quantor ("*") eats as much text
   *   as possible. <br>
   *   <em>false</em>: the all quantor ("*") eats as little text
   *   as possible.
   * @return <em>true</em>: ! _notPattern: the string matches<br>
   *   _notPattern: the string matches not<br>
   *   <em>false</em>: otherwise
   */
  virtual bool search(const char *toTest, int toTestLength = -1,
      Hit *hit = NULL, bool greedy = false);
  /**
   * Defines the patterns.
   * @param patterns The patterns to compile: a list of single patterns.
   *   @see separator.
   * @param patternsLength -1: <em>strlen(patterns)</em> is used.<br>
   *   Otherwise: the length of <em>patterns</em>
   * @param ignoreCase <em>true</em>: The case is ignored while comparison.
   * @param separator The separator of the different patterns.
   *   If <em>nullptr</em>: the first char of <em>patterns</em> is used as separator.
   * @param notPrefix A string that makes a pattern to a "not pattern":
   *   Search is successful if the pattern does not match.
   */
  void set(const char *patterns, int patternsLength = -1, bool ignoreCase =
      false, const char *separator = NULL, const char *notPrefix = "-");
  const char* status(std::string &buffer, const char *prefix) const;
  /** Sets the mode whether the case will be ignored or not.
   * @param onNotOff	<em>true</em>: the case will be ignored.
   */
  void setIgnoreCase(bool onNotOff) {
    for (size_t ii = 0; ii < _patterns.size(); ii++)
      _patterns[ii]->setIgnoreCase(onNotOff);
  }
private:
  int setOne(int index, const char *pattern, size_t patternLength,
      bool ignoreCase, const std::string &notPrefix);
};

} /* cppknife */

#endif /* CORE_MATCHER_HPP_ */
