/*
 * LineAgent.hpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_LINEAGENT_HPP_
#define TEXT_LINEAGENT_HPP_
namespace cppknife {

/**
 * @brief Allows splitting a file into lines like <em>FILE</em> with the possibility to detect binary data.
 */
class LineAgent {
public:
  LineAgent(Logger *logger);
  virtual ~LineAgent();
public:
  /**
   * Tests whether the end of file has been reached: No more data available.
   */
  bool eof() const {
    return _eofReached && _restLength == 0;
  }
  /**
   * Returns whether binary data has been read.
   */
  inline bool hasBinaryData() const {
    return _hasBinaryData;
  }
  /**
   * Returns the next line.
   * Reads some data into the <em>_buffer</em> if needed.
   * @param[out] length The line length.
   * @return <em>nullptr</em>: no more data.
   *   Otherwise: the line without ending '\n'.
   */
  const char* nextLine(size_t &length);
  /**
   * Opens a file.
   * @param filename The name of the file
   * @return true: success
   */
  bool openFile(const char *filename);
  void reset();
  /**
   * Sets the buffer size. This is the maximal possible line length.
   * @param bufferSize the new buffersize.
   */
  void setBufferSize(size_t bufferSize);
protected:
  void fillBuffer(const char *start);

protected:
  Logger *_logger;
  int _handle;
  std::string _filename;
  char _staticBuffer[64000];
  char *_buffer;
  size_t _bufferSize;
  char *_endOfBuffer;
  char *_nextLine;
  size_t _restLength;
  ///
  /// true: the last fread() has returned 0
  bool _eofReached;
  bool _hasBinaryData;
}
;
} /* namespace */
#endif /* TEXT_LINEAGENT_HPP_ */
