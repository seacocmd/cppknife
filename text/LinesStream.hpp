/*
 * LinesStream.hpp
 *
 *  Created on: 11.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_LINESSTREAM_HPP_
#define TEXT_LINESSTREAM_HPP_
/// Defines an interface (abstract class) delivering lines without holding all lines in a list.
namespace cppknife {

/**
 * @brief Defines an interface (abstract class) delivering lines without holding all lines in a list.
 *
 * It automatically fetches the next line if needed.
 */
class LinesStream {
public:
  virtual ~LinesStream();
public:
  /**
   * Returns whether the end of input is reached.
   * @return <em>true</em>No more input is available.
   */
  virtual bool endOfInput() const = 0;
  /**
   * Returns the next piece of text.
   * @param text OUT: the next piece of text.
   * @return <em>true</em> text was available.
   */
  virtual bool fetch(std::string &text) = 0;
  virtual std::string name() const;
};

/**
 * @brief The implementation of <em>LinesStream</em> for a complete text.
 */
class StringLinesStream: public LinesStream {
protected:
  std::string _text;
  bool _valid;
public:
  StringLinesStream(const char *text);
  StringLinesStream(const std::string &text);
  virtual ~StringLinesStream();
public:
  virtual bool endOfInput() const;
  virtual bool fetch(std::string &text);
};

/**
 * @brief The implementation of <em>LinesStream</em> for a text file.
 */
class FileLinesStream: public LinesStream, public LineReader {
protected:
  bool _endOfInput;
public:
  FileLinesStream(const char *filename, Logger &logger, bool removeNewline =
      true, size_t bufferSize = 0x10000);
  virtual ~FileLinesStream();
public:
  virtual bool endOfInput() const;
  virtual bool fetch(std::string &text);
  virtual std::string name() const {
    return _filename;
  }
};
} /* namespace cppknife */

#endif /* TEXT_LINESSTREAM_HPP_ */
