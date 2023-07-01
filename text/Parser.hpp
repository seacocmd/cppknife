/*
 * Parser.hpp
 *
 *  Created on: 03.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_PARSER_HPP_
#define TEXT_PARSER_HPP_

namespace cppknife {

enum TokenType {
  TT_UNKNOWN,
  TT_NUMBER,
  TT_WORD,
  TT_STRING,
  TT_OPERATOR,
  TT_KEYWORD,
  TT_IDENTIFIER,
  TT_COMMENT,
  TT_EOF,
  TT_SPECIAL_1,
  TT_SPECIAL_2,
  TT_SPECIAL_3,
  TT_SPECIAL_4,
  TT_SPECIAL_5,
  TT_SPECIAL_6,
  TT_SPECIAL_7,
  TT_SPECIAL_8,
  TT_SPECIAL_9,
  TT_SPECIAL_10,
};
class AbortException {
public:
  int _exitCode;
public:
  AbortException(int exitCode) :
      _exitCode(exitCode) {
  }
};
class Token {
public:
  std::string _string;
  TokenType _type;
  int _keywordId;
public:
  Token() :
      _string(), _type(TT_UNKNOWN), _keywordId(-1) {
  }
  ~Token() {
  }
  ;
};

class InputData {
public:
  std::string _line;
  std::string _unprocessed;
  size_t _lineNo;
  std::string _filename;
  InputData() :
      _line(), _unprocessed(), _lineNo(0), _filename() {
  }
  ~InputData() {
  }
};
class Parser {
protected:
  InputData _input;
  Logger &_logger;
  Token _token;
  Token _nextToken;
  std::vector<std::string> _keywords;
  std::vector<const std::regex*> _reSpecials;
public:
  std::regex const &_reWhitespaces;
  std::regex const &_reId;
  std::regex const &_reInt;
  std::regex const &_reFloat;
  std::regex &_reOperator1;
  std::regex &_reOperator2;
  std::regex const &_reOneLineComment;
  std::regex const &_reCommentStart;
  std::regex const &_reCommentEnd;
public:
  static const std::regex _regexNewline;
  static const std::regex _regexWhitespaces;
  static const std::regex _regexId;
  static const std::regex _regexInt;
  static const std::regex _regexFloat;
  static const std::regex _regexOperator1;
  static const std::regex _regexOperator2;
  static const std::regex _regexOneLineComment;
  static const std::regex _regexCommentStart;
  static const std::regex _regexCommentEnd;
  static const std::regex _regexLineEnd;
public:
  Parser(Logger &logger);
  virtual ~Parser();
public:
  /**
   * Adds a regular expression for a special syntax construct.
   * It will be used in parseSpecial().
   * @param regExpr The regular expression.
   */
  void addSpecialTokenRegex(const std::regex &regExpr);
  /**
   * Checks the current token. If it is not one of the specified tokens an exception is thrown.
   * @param expectedType  The token type of the next token.
   * @param expectedStrings A list of all allowed token values separated by blanks,
   *  preceded and followed by a blank. Example: " < > <=  >= ". May be <em>nullptr</em>
   */
  void assertCurrentToken(TokenType expectedType, const char *expectedStrings =
      nullptr);
  /**
   * Parses the next token. If it is not one of the specified tokens an exception is thrown.
   * @param expectedType  The token type of the next token.
   * @param expectedStrings A list of all allowed token values separated by blanks,
   *  preceded and followed by a blank. Example: " < > <=  >= ". May be <em>nullptr</em>
   */
  void assertToken(TokenType expectedType,
      const char *expectedStrings = nullptr);
  /**
   * Returns the current input data.
   */
  inline InputData& input() {
    return _input;
  }
  /**
   * Skips the whitespaces and returns the next character without removing it from the input.
   */
  char firstChar();
  /**
   * Tests whether the top of the unprocessed data contains a given string (word).
   * If true the word will be parsed.
   * @param word That string is compared with the head of the unprocessed data.
   * @return <em>true</em>: the <em>word</em> has been found and is "eaten" (removed from the unprocessed data).
   */
  bool hasWaitingWord(const char *word);
  /**
   * Returns the index of a given list where the current token is found or -1.
   * @param words A list of words, ending with <em>nullptr</em>.
   * @return -1: not found. Otherwise: the index of <em>words</em> whith <em>words[index]</em> is the current token.
   */
  int indexOfWords(const char *words[]);
  /**
   * Tests whether the current token is a given word.
   * @param word The word to test.
   * @return <em>bool</em>: the current token is the word.
   */
  bool isWord(const char *word);
  /**
   * Tests whether the current token is a given operator.
   * @param theOperator The expected operartor
   * @return <em>true</em>: the current token is the given operator.
   */
  bool isOperator(const char *theOperator);
  /**
   * Determines the next token without changing the "normal" parser state.
   */
  TokenType lookAhead();
  /**
   * Returns the result of lookAhead().
   */
  inline const Token& nextToken() {
    return _nextToken;
  }
  /**
   * Parses the next token.
   * @return The token type of the found token.
   */
  TokenType parse();
  /**
   * Parses an integer value. If there is no integer value the <em>defaultValue</em> is returned.
   * @param name The name of the parameter: for error logging.
   * @param defaultValue That value is returned if the top of the unprocessed data does not contain an integer.
   * @param needed <em>true</em>: if there is no integer an error occurs.
   * @return <em>defaultValue</em>: the unprocessed data does not start with an integer. Otherwise: the found integer.
   */
  int parseInt(const char *name, int defaultValue, bool needed);
  /**
   * Parses customized token types defined by regular expressions.
   * @param regExpr1 <em>nullptr</em>: <em>_reSpecials</em> will be used. Otherwise: that regular expression will be tested.
   * @param tokenType1 The result if <em>regExpr1</em> is found.
   * @param regExpr2 If not <em>nullptr</em> and <em>regExpr1</em> does not match: That regular expression will be tested.
   * @param tokenType2 The result if <em>regExpr2</em> is found.
   * @return <em>tokenType</em>: <em>regExpr</em> has been found.
   *  TT_SPECIAL_1 + ix, where ix is the index of the regular expression.
   */
  TokenType parseSpecial(const std::regex *regExpr1 = nullptr, int tokenType1 =
      0, const std::regex *regExpr2 = nullptr, int tokenType2 = 0);
  /**
   * Prepares the error message of a parser error.
   * @param message The error message. Will be expanded by filename and line no.
   * @return The expanded error message.
   */
  const std::string prepareError(const char *message) const;
  /**
   * Defines the input of the parsing process.
   * @param input The string that will be parsed.
   * @param lineNo The line number of the error: <em>-1</em> if not relevant.
   * @param filename The name of the file with the error. <em>nullptr</em> if not relevant.
   */
  void setInput(const char *input, int lineNo = -1, const char *filename =
      nullptr);
  /**
   * Defines the keywords. If one is found the token type is set to TT_KEYWORD
   * and the value of <em>_token.keywordId</em> is the index of the found keyword in <em>_keywords</em>.
   * @
   */
  void setKeywords(const std::vector<std::string> &keywords);
  /**
   * Removes the first character of the unprocessed input. @see firstChar().
   */
  void skipFirstChar();
  /**
   * Returns the result of the last call of nextToken().
   */
  inline const Token& token() const {
    return _token;
  }
  /**
   * Returns the token as the index of a sorted word list.
   * @param words A list of sorted keywords.
   */
  int tokenAsIndex(const std::vector<std::string> &words) const;
  /**
   * Returns the token string with translated meta characters.
   */
  std::string tokenAsInterpolatedString() const;
  /**
   * Returns the token string without string delimiter.
   */
  std::string tokenAsRawString() const;
  /**
   *  Returns the unmodified token string as string.
   */
  const std::string& tokenAsString() const;
  /**
   *  Returns the unmodified token string as const char pointer.
   */
  const char* tokenAsCString() const;
public:
  static const char* typeName(TokenType type);
};

} /* namespace cppknife */

#endif /* TEXT_PARSER_HPP_ */
